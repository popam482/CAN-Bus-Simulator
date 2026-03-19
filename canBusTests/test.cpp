#include "pch.h"
#include "EngineECU.h"
#include "CANBus.h"
#include "BrakeECU.h"

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
