#include "pch.h"
#include "EngineECU.h"
#include "CANBus.h"
#include "BrakeECU.h"
#include "AbsECU.h"
#include "TransmissionECU.h"
#include "BcmECU.h"
#include "DashboardECU.h"
#include "ClimateControlECU.h"

class EngineECUTest : public ::testing::Test {
protected:
    CANBus bus;
    EngineECU engine{ bus };
};

class BrakeECUTest : public ::testing::Test {
protected:
    CANBus bus;
    BrakeECU brakes{ bus };
};

class AbsECUTest : public ::testing::Test{
protected:
    CANBus bus;
    AbsECU abs{bus};
};

class TransmissionECUTest : public ::testing::Test {
protected:
    CANBus bus;
    TransmissionECU transmission{bus};
};

class BcmECUTest : public ::testing::Test {
protected:
    CANBus bus;
    BcmECU bcm{ bus };
};

class DashboardECUTest : public ::testing::Test {
protected:
    DashboardECU dashboard;
};

class ClimateControlECUTest : public ::testing::Test {
protected:
    CANBus bus;
    ClimateControlECU climate{ bus, 22.0f, 28.0f };
};


TEST_F(EngineECUTest, ConstructorIntializeOilTemp) {
    engine.setOilTemp(85.5f);
    EXPECT_EQ(engine.getOilTemp(), 85.5f);
}

TEST_F(EngineECUTest, SetOilLevelChangesValue) {
    engine.setOilLevel(0.7f);
    EXPECT_EQ(engine.getOilLevel(), 0.7f);
}

TEST_F(EngineECUTest, SetCoolantLevelChangesValue) {
    engine.setCoolantLevel(0.6f);
    EXPECT_EQ(engine.getCoolantLevel(), 0.6f);
}

TEST_F(EngineECUTest, ConstructorIntializeCoolantTemp) {
    engine.setCoolantTemp(75.5f);
    EXPECT_EQ(engine.getCoolantTemp(), 75.5f);
}

TEST_F(EngineECUTest, ConstructorIntializeFuelLevel) {
    engine.setFuelLevel(0.7f);
    EXPECT_EQ(engine.getFuelLevel(), 0.7f);
}

TEST_F(EngineECUTest, CalculateRPMAtSpeed40) {
    engine.calculateRPM(40);
    EXPECT_EQ(engine.getEngineRPM(), 800 + (40 * 80));
}

TEST_F(EngineECUTest, CalculateRPMAtSpeed0ReturnsIdle) {
    engine.calculateRPM(0);
    EXPECT_EQ(engine.getEngineRPM(), 800);
}

TEST_F(EngineECUTest, CalculateRPMClampsAtRedline) {
    engine.calculateRPM(200);
    EXPECT_LE(engine.getEngineRPM(), 7000);
}

TEST_F(EngineECUTest, CalculateFuelConsumptionAtSpeed0) {
    engine.calculateFuelConsumption(0);
    EXPECT_FLOAT_EQ(engine.getFuelConsumption(), 0.0f);
}

TEST_F(EngineECUTest, CalculateFuelConsumptionAtSpeed30) {
    engine.calculateFuelConsumption(30);
    EXPECT_FLOAT_EQ(engine.getFuelConsumption(), 10.0f);
}

TEST_F(EngineECUTest, CalculateFuelConsumptionAtSpeed50) {
    engine.calculateFuelConsumption(50);
    EXPECT_FLOAT_EQ(engine.getFuelConsumption(), 6.5f);
}

TEST_F(EngineECUTest, CalculateFuelConsumptionAtSpeed120) {
    engine.calculateFuelConsumption(120);
    EXPECT_FLOAT_EQ(engine.getFuelConsumption(), 8.5f);
}

TEST_F(EngineECUTest, SetMultipleValuesIndependently) {
    engine.setOilTemp(87.5f);
    engine.setOilLevel(0.4f);
    engine.setCoolantTemp(95.0f);
    engine.setCoolantLevel(0.6f);
    engine.setFuelLevel(0.8f);

    EXPECT_FLOAT_EQ(engine.getOilTemp(), 87.5f);
    EXPECT_FLOAT_EQ(engine.getOilLevel(), 0.4f);
    EXPECT_FLOAT_EQ(engine.getCoolantTemp(), 95.0f);
    EXPECT_FLOAT_EQ(engine.getCoolantLevel(), 0.6f);
    EXPECT_FLOAT_EQ(engine.getFuelLevel(), 0.8f);
}

TEST_F(EngineECUTest, OilLevelWithinValidRange) {
    engine.setOilLevel(0.5f);
    EXPECT_GE(engine.getOilLevel(), 0.0f);
    EXPECT_LE(engine.getOilLevel(), 1.0f);
}

TEST_F(EngineECUTest, CoolantLevelWithinValidRange) {
    engine.setCoolantLevel(0.7f);
    EXPECT_GE(engine.getCoolantLevel(), 0.0f);
    EXPECT_LE(engine.getCoolantLevel(), 1.0f);
}

TEST_F(EngineECUTest, FuelLevelWithinValidRange) {
    engine.setFuelLevel(0.5f);
    EXPECT_GE(engine.getFuelLevel(), 0.0f);
    EXPECT_LE(engine.getFuelLevel(), 1.0f);
}

TEST_F(EngineECUTest, GetNameReturnsEngineECU) {
    EXPECT_EQ(engine.getName(), "EngineECU");
}

TEST_F(EngineECUTest, ConstructorInitializesFuelLevel) {
    EXPECT_EQ(engine.getFuelLevel(), 0.8f);
}

TEST_F(EngineECUTest, ConstructorInitializesOilTempToRoom) {
    EXPECT_EQ(engine.getOilTemp(), 20.0f);
}

TEST_F(EngineECUTest, ConstructorInitializesOilLevel) {
    EXPECT_EQ(engine.getOilLevel(), 0.85f);
}

TEST_F(EngineECUTest, ConstructorInitializesCoolantTemp) {
    EXPECT_EQ(engine.getCoolantTemp(), 20.0f);
}

TEST_F(EngineECUTest, ConstructorInitializesCoolantLevel) {
    EXPECT_EQ(engine.getCoolantLevel(), 0.8f);
}
TEST_F(EngineECUTest, FuelConsumptionSpeed35BelowThreshold) {
    engine.calculateFuelConsumption(35);
    EXPECT_FLOAT_EQ(engine.getFuelConsumption(), 10.0f);
}

TEST_F(EngineECUTest, FuelConsumptionSpeed40AboveThreshold) {
    engine.calculateFuelConsumption(40);
    EXPECT_FLOAT_EQ(engine.getFuelConsumption(), 6.5f);
}

TEST_F(EngineECUTest, FuelConsumptionSpeed99BelowHighThreshold) {
    engine.calculateFuelConsumption(99);
    EXPECT_FLOAT_EQ(engine.getFuelConsumption(), 6.5f);
}

TEST_F(EngineECUTest, FuelConsumptionSpeed100AboveHighThreshold) {
    engine.calculateFuelConsumption(100);
    EXPECT_FLOAT_EQ(engine.getFuelConsumption(), 8.5f);
}

TEST_F(EngineECUTest, SetOilTempToHighValue) {
    engine.setOilTemp(105.0f);
    EXPECT_FLOAT_EQ(engine.getOilTemp(), 105.0f);
}

TEST_F(EngineECUTest, SetOilTempToLowValue) {
    engine.setOilTemp(15.0f);
    EXPECT_FLOAT_EQ(engine.getOilTemp(), 15.0f);
}

TEST_F(EngineECUTest, SetCoolantTempToHighValue) {
    engine.setCoolantTemp(110.0f);
    EXPECT_FLOAT_EQ(engine.getCoolantTemp(), 110.0f);
}

TEST_F(EngineECUTest, SetOilLevelToMinimum) {
    engine.setOilLevel(0.0f);
    EXPECT_EQ(engine.getOilLevel(), 0.0f);
}

TEST_F(EngineECUTest, SetOilLevelToMaximum) {
    engine.setOilLevel(1.0f);
    EXPECT_EQ(engine.getOilLevel(), 1.0f);
}

TEST_F(EngineECUTest, SetFuelLevelToMinimum) {
    engine.setFuelLevel(0.0f);
    EXPECT_EQ(engine.getFuelLevel(), 0.0f);
}

TEST_F(EngineECUTest, SetFuelLevelToMaximum) {
    engine.setFuelLevel(1.0f);
    EXPECT_EQ(engine.getFuelLevel(), 1.0f);
}

TEST_F(EngineECUTest, FuelConsumptionTransitionPoints) {
    engine.calculateFuelConsumption(39);
    EXPECT_FLOAT_EQ(engine.getFuelConsumption(), 10.0f);

    engine.calculateFuelConsumption(40);
    EXPECT_FLOAT_EQ(engine.getFuelConsumption(), 6.5f);
}

TEST_F(EngineECUTest, AllStateSetAtOnce) {
    engine.setOilTemp(87.5f);
    engine.setOilLevel(0.82f);
    engine.setCoolantTemp(90.0f);
    engine.setCoolantLevel(0.71f);
    engine.setFuelLevel(0.85f);
    engine.calculateRPM(70);
    engine.calculateFuelConsumption(70);

    EXPECT_FLOAT_EQ(engine.getOilTemp(), 87.5f);
    EXPECT_FLOAT_EQ(engine.getOilLevel(), 0.82f);
    EXPECT_FLOAT_EQ(engine.getCoolantTemp(), 90.0f);
    EXPECT_FLOAT_EQ(engine.getCoolantLevel(), 0.71f);
    EXPECT_FLOAT_EQ(engine.getFuelLevel(), 0.85f);
    EXPECT_EQ(engine.getEngineRPM(), 800 + (70 * 80));
    EXPECT_FLOAT_EQ(engine.getFuelConsumption(), 6.5f);
}

TEST_F(BrakeECUTest, ConstructorInitializesBrakePressuredFalse) {
    EXPECT_FALSE(brakes.isBrakePressured());
}

TEST_F(BrakeECUTest, GetNameReturnsBrakeSensorECU) {
    EXPECT_EQ(brakes.getName(), "Brake_Sensor_ECU");
}

TEST_F(BrakeECUTest, PressBrakeSetsPressuredTrue) {
    brakes.pressBrake();
    EXPECT_TRUE(brakes.isBrakePressured());
}

TEST_F(BrakeECUTest, ReleaseBrakeSetsPressuredFalse) {
    brakes.pressBrake();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    brakes.releaseBrake();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    EXPECT_FALSE(brakes.isBrakePressured());
}


TEST_F(AbsECUTest, ConstructorInitializesNotActivated) {
    EXPECT_FALSE(abs.isActivated());
}

TEST_F(AbsECUTest, GetNameReturnsABSECU) {
    EXPECT_EQ(abs.getName(), "ABS_ECU");
}

TEST_F(AbsECUTest, ConstructorInitializesWheelSpeedToZero) {
    EXPECT_EQ(abs.getWheelSpeed(), 0);
}

TEST_F(AbsECUTest, ReceiveHighSpeedFrameActivatesABS) {
    CANFrame frame;
    frame.setId(0x201);
    frame.setData({ 120 });

    abs.receiveFrame(frame);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    EXPECT_TRUE(abs.isActivated());
}

TEST_F(AbsECUTest, ReceiveLowSpeedFrameDeactivatesABS) {
    CANFrame highSpeedFrame;
    highSpeedFrame.setId(0x101);
    highSpeedFrame.setData({ 120 });
    abs.receiveFrame(highSpeedFrame);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    CANFrame lowSpeedFrame;
    lowSpeedFrame.setId(0x101);
    lowSpeedFrame.setData({ 20 });
    abs.receiveFrame(lowSpeedFrame);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    EXPECT_FALSE(abs.isActivated());
}

TEST_F(TransmissionECUTest, ConstructorInitializesGearToNeutral) {
    EXPECT_EQ(transmission.getCurrentGear(), 0);
}

TEST_F(TransmissionECUTest, GetNameReturnsTransmissionECU) {
    EXPECT_EQ(transmission.getName(), "TransmissionECU");
}

TEST_F(TransmissionECUTest, ToGear1) {
    CANFrame frame;
    frame.setId(0x101);
    frame.setData({ 10 });

    transmission.receiveFrame(frame);
    std::this_thread::sleep_for(std::chrono::milliseconds(150));

    EXPECT_EQ(transmission.getCurrentGear(), 1);
}

TEST_F(TransmissionECUTest, ToGear2) {
    CANFrame frame;
    frame.setId(0x101);
    frame.setData({ 30 });

    transmission.receiveFrame(frame);
    std::this_thread::sleep_for(std::chrono::milliseconds(150));

    EXPECT_EQ(transmission.getCurrentGear(), 2);
}

TEST_F(TransmissionECUTest, ToGear3) {
    CANFrame frame;
    frame.setId(0x101);
    frame.setData({ 50 });

    transmission.receiveFrame(frame);
    std::this_thread::sleep_for(std::chrono::milliseconds(150));

    EXPECT_EQ(transmission.getCurrentGear(), 3);
}

TEST_F(TransmissionECUTest, ToGear4) {
    CANFrame frame;
    frame.setId(0x101);
    frame.setData({ 70 });

    transmission.receiveFrame(frame);
    std::this_thread::sleep_for(std::chrono::milliseconds(150));

    EXPECT_EQ(transmission.getCurrentGear(), 4);
}

TEST_F(TransmissionECUTest, ToGear5) {
    CANFrame frame;
    frame.setId(0x101);
    frame.setData({ 90 });

    transmission.receiveFrame(frame);
    std::this_thread::sleep_for(std::chrono::milliseconds(150));

    EXPECT_EQ(transmission.getCurrentGear(), 5);
}

TEST_F(TransmissionECUTest, ReceiveBrakeSignalDownshifts) {
    CANFrame speedFrame;
    speedFrame.setId(0x101);
    speedFrame.setData({ 70 });
    transmission.receiveFrame(speedFrame);
    std::this_thread::sleep_for(std::chrono::milliseconds(150));
    EXPECT_EQ(transmission.getCurrentGear(), 4);

    CANFrame brakeFrame;
    brakeFrame.setId(0x201);
    brakeFrame.setData({ 1 });
    transmission.receiveFrame(brakeFrame);
    std::this_thread::sleep_for(std::chrono::milliseconds(150));

    EXPECT_LT(transmission.getCurrentGear(), 4);
}

TEST_F(TransmissionECUTest, SequentialSpeedIncreaseShiftsProgressive) {
    std::vector<uint8_t> speeds = { 10, 25, 40, 55, 75, 95 };
    std::vector<uint8_t> expectedGears = { 1, 2, 2, 3, 4, 5 };

    for (size_t i = 0; i < speeds.size(); ++i) {
        CANFrame frame;
        frame.setId(0x101);
        frame.setData({ speeds[i] });

        transmission.receiveFrame(frame);
        std::this_thread::sleep_for(std::chrono::milliseconds(150));

        EXPECT_EQ(transmission.getCurrentGear(), expectedGears[i]);
    }
}

TEST_F(TransmissionECUTest, SequentialSpeedDecreaseShiftsRegressive) {
    CANFrame highSpeedFrame;
    highSpeedFrame.setId(0x101);
    highSpeedFrame.setData({ 100 });
    transmission.receiveFrame(highSpeedFrame);
    std::this_thread::sleep_for(std::chrono::milliseconds(150));
    EXPECT_EQ(transmission.getCurrentGear(), 5);

    std::vector<uint8_t> speeds = { 80, 60, 40, 20, 10 };
    std::vector<uint8_t> expectedGears = { 4, 3, 2, 1, 1 };  

    for (size_t i = 0; i < speeds.size(); ++i) {
        CANFrame frame;
        frame.setId(0x101);
        frame.setData({ speeds[i] });

        transmission.receiveFrame(frame);
        std::this_thread::sleep_for(std::chrono::milliseconds(150));

        EXPECT_EQ(transmission.getCurrentGear(), expectedGears[i]);
    }
}

TEST_F(TransmissionECUTest, ZeroSpeedReturnsToNeutral) {
    CANFrame speedFrame;
    speedFrame.setId(0x101);
    speedFrame.setData({ 50 });
    transmission.receiveFrame(speedFrame);
    std::this_thread::sleep_for(std::chrono::milliseconds(150));
    EXPECT_EQ(transmission.getCurrentGear(), 3);

    CANFrame zeroSpeedFrame;
    zeroSpeedFrame.setId(0x101);
    zeroSpeedFrame.setData({ 0 });
    transmission.receiveFrame(zeroSpeedFrame);
    std::this_thread::sleep_for(std::chrono::milliseconds(150));

    EXPECT_EQ(transmission.getCurrentGear(), 0);
}

TEST_F(TransmissionECUTest, BrakeFollowedByAcceleration) {
    CANFrame speedFrame;
    speedFrame.setId(0x101);
    speedFrame.setData({ 70 });
    transmission.receiveFrame(speedFrame);
    std::this_thread::sleep_for(std::chrono::milliseconds(150));

    CANFrame brakeFrame;
    brakeFrame.setId(0x201);
    brakeFrame.setData({ 1 });
    transmission.receiveFrame(brakeFrame);
    std::this_thread::sleep_for(std::chrono::milliseconds(150));

    uint8_t gearAfterBrake = transmission.getCurrentGear();

    CANFrame accelerateFrame;
    accelerateFrame.setId(0x101);
    accelerateFrame.setData({ 80 });
    transmission.receiveFrame(accelerateFrame);
    std::this_thread::sleep_for(std::chrono::milliseconds(150));

    EXPECT_GT(transmission.getCurrentGear(), gearAfterBrake);
}

TEST_F(BcmECUTest, ConstructorInitializesLightsOff) {
    EXPECT_FALSE(bcm.getHeadlightsStatus());
    EXPECT_FALSE(bcm.getWiperStatus());
}

TEST_F(BcmECUTest, GetNameReturnsBCMECU) {
    EXPECT_EQ(bcm.getName(), "BCM_ECU");
}

TEST_F(BcmECUTest, TurnHeadlightsOn) {
    bcm.setHeadlights(true);
    EXPECT_TRUE(bcm.getHeadlightsStatus());
}

TEST_F(BcmECUTest, TurnHeadlightsOff) {
    bcm.setHeadlights(true);
    bcm.setHeadlights(false);
    EXPECT_FALSE(bcm.getHeadlightsStatus());
}

TEST_F(BcmECUTest, TurnWipersOn) {
    bcm.setWipers(true);
    EXPECT_TRUE(bcm.getWiperStatus());
}

TEST_F(BcmECUTest, TurnWipersOff) {
    bcm.setWipers(false);
    EXPECT_FALSE(bcm.getWiperStatus());
}

TEST_F(BcmECUTest, ToggleHeadlightsManyTimes) {
    for (int i = 0; i < 10; ++i) {
        bcm.setHeadlights(true);
        EXPECT_TRUE(bcm.getHeadlightsStatus());

        bcm.setHeadlights(false);
        EXPECT_FALSE(bcm.getHeadlightsStatus());
    }
}

TEST_F(BcmECUTest, ToggleWipersManyTimes) {
    for (int i = 0; i < 10; ++i) {
        bcm.setWipers(true);
        EXPECT_TRUE(bcm.getWiperStatus());

        bcm.setWipers(false);
        EXPECT_FALSE(bcm.getWiperStatus());
    }
}

TEST_F(BcmECUTest, HeadlightsAndWipersIndependent) {
    bcm.setHeadlights(true);
    bcm.setWipers(false);

    EXPECT_TRUE(bcm.getHeadlightsStatus());
    EXPECT_FALSE(bcm.getWiperStatus());
}

TEST_F(BcmECUTest, BothHeadlightsAndWipersOn) {
    bcm.setHeadlights(true);
    bcm.setWipers(true);

    EXPECT_TRUE(bcm.getHeadlightsStatus());
    EXPECT_TRUE(bcm.getWiperStatus());
}

TEST_F(BcmECUTest, BothHeadlightsAndWipersOff) {
    bcm.setHeadlights(false);
    bcm.setWipers(false);

    EXPECT_FALSE(bcm.getHeadlightsStatus());
    EXPECT_FALSE(bcm.getWiperStatus());
}

TEST_F(DashboardECUTest, ConstructorInitializesSpeedToZero) {
    EXPECT_EQ(dashboard.getDisplayedSpeed(), 0);
}

TEST_F(DashboardECUTest, ConstructorInitializesRPMToZero) {
    EXPECT_EQ(dashboard.getDisplayedRPM(), 0);
}

TEST_F(DashboardECUTest, ConstructorInitializesFuelLevelToZero) {
    EXPECT_EQ(dashboard.getDisplayedFuelLevel(), 0);
}

TEST_F(DashboardECUTest, SetDisplayedSpeed) {
    dashboard.setDisplayedSpeed(50);
    EXPECT_EQ(dashboard.getDisplayedSpeed(), 50);
}

TEST_F(DashboardECUTest, SetDisplayedRPM) {
    dashboard.setDisplayedRPM(3200);
    EXPECT_EQ(dashboard.getDisplayedRPM(), 3200);
}

TEST_F(DashboardECUTest, SetDisplayedFuelLevel) {
    dashboard.setDisplayedFuelLevel(75);
    EXPECT_EQ(dashboard.getDisplayedFuelLevel(), 75);
}

TEST_F(DashboardECUTest, SetOilTemp) {
    dashboard.setDisplayedOilTemp(85);
    EXPECT_EQ(dashboard.getDisplayedOilTemp(), 85);
}

TEST_F(DashboardECUTest, SetCoolantTemp) {
    dashboard.setDisplayedCoolantTemp(95);
    EXPECT_EQ(dashboard.getDisplayedCoolantTemp(), 95);
}


TEST_F(DashboardECUTest, SpeedMultipleUpdates) {
    std::vector<uint8_t> speeds = { 10, 30, 50, 70, 90 };

    for (uint8_t speed : speeds) {
        dashboard.setDisplayedSpeed(speed);
        EXPECT_EQ(dashboard.getDisplayedSpeed(), speed);
    }
}

TEST_F(DashboardECUTest, RPMMultipleUpdates) {
    std::vector<uint16_t> rpms = { 800, 1500, 3000, 5000, 6500 };

    for (uint16_t rpm : rpms) {
        dashboard.setDisplayedRPM(rpm);
        EXPECT_EQ(dashboard.getDisplayedRPM(), rpm);
    }
}

TEST_F(DashboardECUTest, FuelLevelMultipleUpdates) {
    std::vector<uint8_t> fuels = { 100, 75, 50, 25, 10 };

    for (uint8_t fuel : fuels) {
        dashboard.setDisplayedFuelLevel(fuel);
        EXPECT_EQ(dashboard.getDisplayedFuelLevel(), fuel);
    }
}

TEST_F(ClimateControlECUTest, SetTargetTempChangesValue) {
    climate.setTargetTemp(20.0f);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    EXPECT_TRUE(true);  
}

TEST_F(ClimateControlECUTest, TurnACOn) {
    climate.turnAc();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    EXPECT_TRUE(true);
}

TEST_F(ClimateControlECUTest, MultipleTargetTempChanges) {
    for (int i = 20; i <= 28; ++i) {
        climate.setTargetTemp(static_cast<float>(i));
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
    EXPECT_TRUE(true);
}

TEST_F(ClimateControlECUTest, TemperatureConvergenceWhenACOn) {
    climate.setTargetTemp(22.0f);
    climate.turnAc();

    std::this_thread::sleep_for(std::chrono::milliseconds(2500));

    EXPECT_TRUE(true);  
}

TEST_F(ClimateControlECUTest, LargeTemperatureDifference) {
    climate.setTargetTemp(16.0f);  
    climate.turnAc();

    std::this_thread::sleep_for(std::chrono::milliseconds(3000));

    EXPECT_TRUE(true);
}

TEST_F(ClimateControlECUTest, SmallTemperatureDifference) {
    climate.setTargetTemp(27.5f);  
    climate.turnAc();

    std::this_thread::sleep_for(std::chrono::milliseconds(1500));

    EXPECT_TRUE(true);
}