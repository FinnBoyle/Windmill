type Metrics = {
  id: string;
  memory: number;
  voltage: number;
  stepperState: number;
  rpm: number;
  kp: number;
  ki: number;
  kd: number;
};

type PIDData = {
  id: string;
  interval: number;
  rotations: number[];
  errors: number[];
};
type Settings = {
  stepperState: number;
  rpm?: number;
  steps?: number;
  kp?: number;
  ki?: number;
  kd?: number;
};
export { Metrics, PIDData, Settings };
