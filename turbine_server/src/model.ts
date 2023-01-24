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
type Settings = {
  stepperState: number;
  rpm?: number;
  steps?: number;
  kp?: number;
  ki?: number;
  kd?: number;
};
export { Metrics, Settings };
