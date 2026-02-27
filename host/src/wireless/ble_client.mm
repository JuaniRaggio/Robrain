#include "ble_client.h"
#include "protocol/wireless_packet.h"
#import <CoreBluetooth/CoreBluetooth.h>
#import <Foundation/Foundation.h>
#include <atomic>
#include <chrono>
#include <thread>

@class BLEDelegate;

namespace robrain {
struct BleClient::Impl {
    serial::Consumer<wireless_protocol::MotorPayload, BleClient::queue_capacity>& consumer;
    BLEDelegate* delegate;
    dispatch_queue_t ble_queue;
    std::thread sender_thread;
    std::atomic<bool> running{false};
    std::atomic<bool> connected{false};
    std::atomic<bool> ready{false};

    explicit Impl(
        serial::Consumer<wireless_protocol::MotorPayload, BleClient::queue_capacity>& c)
        : consumer(c), delegate(nil), ble_queue(nullptr) {}
};
} // namespace robrain

@interface BLEDelegate : NSObject <CBCentralManagerDelegate, CBPeripheralDelegate>
@property(nonatomic, strong) CBCentralManager* central;
@property(nonatomic, strong) CBPeripheral* connectedPeripheral;
@property(nonatomic, strong) CBCharacteristic* cmdCharacteristic;
@property(nonatomic, assign) robrain::BleClient::Impl* impl;
@end

@implementation BLEDelegate

- (void)centralManagerDidUpdateState:(CBCentralManager*)central {
    if (central.state == CBManagerStatePoweredOn && self.impl->running.load()) {
        CBUUID* serviceUUID = [CBUUID UUIDWithString:@(wireless_protocol::SERVICE_UUID)];
        [central scanForPeripheralsWithServices:@[ serviceUUID ] options:nil];
        NSLog(@"[BLE] Scanning for %s ...", wireless_protocol::DEVICE_NAME);
    } else {
        NSLog(@"[BLE] Central state: %ld", (long)central.state);
    }
}

- (void)centralManager:(CBCentralManager*)central
    didDiscoverPeripheral:(CBPeripheral*)peripheral
        advertisementData:(NSDictionary<NSString*, id>*)advertisementData
                     RSSI:(NSNumber*)RSSI {

    NSString* name = peripheral.name;
    if (!name) return;

    if (strcmp([name UTF8String], wireless_protocol::DEVICE_NAME) == 0) {
        NSLog(@"[BLE] Found %s (RSSI %@), connecting...", wireless_protocol::DEVICE_NAME, RSSI);
        self.connectedPeripheral = peripheral;
        [central stopScan];
        [central connectPeripheral:peripheral options:nil];
    }
}

- (void)centralManager:(CBCentralManager*)central
    didConnectPeripheral:(CBPeripheral*)peripheral {
    NSLog(@"[BLE] Connected to %s", wireless_protocol::DEVICE_NAME);
    self.impl->connected.store(true);
    peripheral.delegate = self;

    CBUUID* serviceUUID = [CBUUID UUIDWithString:@(wireless_protocol::SERVICE_UUID)];
    [peripheral discoverServices:@[ serviceUUID ]];
}

- (void)peripheral:(CBPeripheral*)peripheral didDiscoverServices:(NSError*)error {
    if (error) {
        NSLog(@"[BLE] Service discovery error: %@", error);
        return;
    }

    CBUUID* cmdUUID = [CBUUID UUIDWithString:@(wireless_protocol::CMD_CHAR_UUID)];
    for (CBService* service in peripheral.services) {
        [peripheral discoverCharacteristics:@[ cmdUUID ] forService:service];
    }
}

- (void)peripheral:(CBPeripheral*)peripheral
    didDiscoverCharacteristicsForService:(CBService*)service
                                   error:(NSError*)error {
    if (error) {
        NSLog(@"[BLE] Characteristic discovery error: %@", error);
        return;
    }

    CBUUID* cmdUUID = [CBUUID UUIDWithString:@(wireless_protocol::CMD_CHAR_UUID)];
    for (CBCharacteristic* c in service.characteristics) {
        if ([c.UUID isEqual:cmdUUID]) {
            self.cmdCharacteristic = c;
            self.impl->ready.store(true);
            NSLog(@"[BLE] Ready -- CMD characteristic found");
            return;
        }
    }
}

- (void)centralManager:(CBCentralManager*)central
    didDisconnectPeripheral:(CBPeripheral*)peripheral
                      error:(NSError*)error {
    NSLog(@"[BLE] Disconnected (error: %@)", error);
    self.impl->ready.store(false);
    self.impl->connected.store(false);
    self.cmdCharacteristic = nil;
    self.connectedPeripheral = nil;

    if (self.impl->running.load()) {
        NSLog(@"[BLE] Re-scanning...");
        CBUUID* serviceUUID = [CBUUID UUIDWithString:@(wireless_protocol::SERVICE_UUID)];
        [central scanForPeripheralsWithServices:@[ serviceUUID ] options:nil];
    }
}

- (void)centralManager:(CBCentralManager*)central
    didFailToConnectPeripheral:(CBPeripheral*)peripheral
                         error:(NSError*)error {
    NSLog(@"[BLE] Failed to connect (error: %@)", error);
    self.connectedPeripheral = nil;

    if (self.impl->running.load()) {
        NSLog(@"[BLE] Re-scanning...");
        CBUUID* serviceUUID = [CBUUID UUIDWithString:@(wireless_protocol::SERVICE_UUID)];
        [central scanForPeripheralsWithServices:@[ serviceUUID ] options:nil];
    }
}

- (void)peripheral:(CBPeripheral*)peripheral
    didWriteValueForCharacteristic:(CBCharacteristic*)characteristic
                             error:(NSError*)error {
    if (error) {
        NSLog(@"[BLE] Write error: %@", error);
    }
}

@end

namespace robrain {

BleClient::BleClient(
    serial::Consumer<wireless_protocol::MotorPayload, queue_capacity>& consumer)
    : impl_(new Impl(consumer)) {}

BleClient::~BleClient() {
    if (impl_->running.load()) {
        stop_async();
    }
    delete impl_;
}

bool BleClient::is_connected() const { return impl_->connected.load(); }

void BleClient::start_async() {
    impl_->running.store(true);
    impl_->ble_queue = dispatch_queue_create("com.robrain.ble", DISPATCH_QUEUE_SERIAL);
    impl_->delegate = [[BLEDelegate alloc] init];
    impl_->delegate.impl = impl_;

    dispatch_sync(impl_->ble_queue, ^{
        impl_->delegate.central =
            [[CBCentralManager alloc] initWithDelegate:impl_->delegate
                                                 queue:impl_->ble_queue];
    });

    impl_->sender_thread = std::thread([this]() {
        using namespace std::chrono_literals;
        wireless_protocol::MotorPayload cmd;

        while (impl_->running.load()) {
            if (impl_->consumer.pop(cmd)) {
                if (impl_->ready.load()) {
                    NSData* data = [NSData dataWithBytes:&cmd
                                                  length:sizeof(cmd)];
                    BLEDelegate* delegate = impl_->delegate;
                    dispatch_async(impl_->ble_queue, ^{
                        if (delegate.cmdCharacteristic && delegate.connectedPeripheral) {
                            [delegate.connectedPeripheral
                                writeValue:data
                                forCharacteristic:delegate.cmdCharacteristic
                                    type:CBCharacteristicWriteWithResponse];
                        }
                    });
                }
                // If not ready, command is discarded
            }
            std::this_thread::sleep_for(10ms);
        }
    });
}

void BleClient::stop_async() {
    impl_->running.store(false);

    if (impl_->sender_thread.joinable()) {
        impl_->sender_thread.join();
    }

    if (impl_->ble_queue) {
        dispatch_sync(impl_->ble_queue, ^{
            [impl_->delegate.central stopScan];
            if (impl_->delegate.connectedPeripheral) {
                [impl_->delegate.central
                    cancelPeripheralConnection:impl_->delegate.connectedPeripheral];
            }
            impl_->delegate.central.delegate = nil;
            impl_->delegate.central = nil;
            impl_->delegate.connectedPeripheral = nil;
            impl_->delegate.cmdCharacteristic = nil;
        });
    }

    impl_->delegate = nil;
    impl_->connected.store(false);
    impl_->ready.store(false);
}

} // namespace robrain
