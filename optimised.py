import math
import time
import random
import logging
from simple_pid import PID
logging.basicConfig(level=logging.INFO)
class WindTurbineSimulator:
    def __init__(self, config):
        self.wind_direction = config['initial_wind_direction']
        self.rotation = 0
        self.prev_time = time.time()
        self.pid = PID(*config['pid_params'], setpoint=config['setpoint'])
        self.wind_update_interval = config['wind_update_interval']
    def _fake_voltage(self):
        """ Simulate voltage based on the turbine's rotation. """
        return math.sin(math.radians(self.rotation)) / 2.0
    def _rotate_turbine(self, change):
        """ Rotate the turbine based on the control value. """
        self.rotation += max(1.8, change)
        self.rotation %= 360
    def update_wind_direction(self):
        """ Update wind direction at random intervals. """
        current_time = time.time()
        if current_time - self.prev_time > self.wind_update_interval:
            self.wind_direction = random.uniform(0, 360)
            self.prev_time = current_time
    def run_simulation(self):
        """ Run the wind turbine simulation."""
        while True:
            self.update_wind_direction()
            voltage = self._fake_voltage()
            control = self.pid(voltage)
            self._rotate_turbine(control)
            logging.info(f"WindDir: {self.wind_direction:.2f}°, "
                         f"Change: {control:.2f}, "
                         f"Rotation: {self.rotation:.2f}°, "
                         f"Voltage: {voltage:.2f}")
            time.sleep(0.1)
if __name__ == "__main__":
    config = {
        'initial_wind_direction': 180,
        'pid_params': (10, 0, 0),
        'setpoint': 0.5,
        'wind_update_interval': 5   # Time in seconds to update wind direction
    }
    simulator = WindTurbineSimulator(config)
    simulator.run_simulation()
