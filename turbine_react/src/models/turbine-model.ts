type Turbine = {
  id: string;
  memory: number;
  voltage: number;
  stepperState: number;
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
  id: string;
  stepperState?: number;
  rpm?: number;
  steps?: number;
  kp?: number;
  ki?: number;
  kd?: number;
};

const getTurbines = async (): Promise<Turbine[]> =>
  fetch("http://localhost:5000/metrics")
    .then((res) => res.json())
    .then((data) => {
      return data as Turbine[];
    });

const getPIDData = async (): Promise<PIDData[]> =>
  fetch("http://localhost:5000/pid")
    .then((res) => res.json())
    .then((data) => {
      return data as PIDData[];
    });

const updateSettings = async (settings: Settings) => {
  return fetch("http://localhost:5000/settings", {
    method: "PUT",
    headers: {
      "Content-Type": "application/json",
    },
    body: JSON.stringify(settings),
  });
};

export { getTurbines, getPIDData, updateSettings };
export type { Turbine, PIDData, Settings };
