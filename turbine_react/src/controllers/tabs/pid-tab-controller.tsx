import PIDTab from "../../components/tabs/pid-tab";
import {
  getPIDData,
  PIDData,
  Settings,
  Turbine,
  updateSettings,
} from "../../models/turbine-model";
import * as React from "react";

type PIDTabProps = {
  selected: Turbine | undefined;
};
type PIDTabState = {
  interval: number;
  rotations: number[];
  errors: number[];
  timePeriod: number;
};
const PIDTabController: React.FC<PIDTabProps> = (props: PIDTabProps) => {
  const [state, setState] = React.useState<PIDTabState>({
    interval: 0,
    rotations: [],
    errors: [],
    timePeriod: 2,
  });
  const onFormSubmit = (e: React.FormEvent<HTMLFormElement>): void => {
    e.preventDefault();

    if (!props.selected) {
      return;
    }

    const { id } = props.selected;
    const form = e.target as HTMLFormElement;
    const kp = parseInt((form.kp as HTMLInputElement).value);
    const ki = parseInt((form.ki as HTMLInputElement).value);
    const kd = parseInt((form.kd as HTMLInputElement).value);
    const seconds = parseInt((form.seconds as HTMLInputElement).value);

    if (isNaN(kp) && isNaN(ki) && isNaN(kd)) {
      setState({
        ...state,
        timePeriod: seconds,
      });
    }

    const fetchData = async () => {
      let settings: Settings = {
        id,
      };
      if (!isNaN(kp)) {
        settings.kp = kp;
      }

      if (!isNaN(ki)) {
        settings.ki = ki;
      }
      if (!isNaN(kd)) {
        settings.kd = kd;
      }
      if (id !== "") {
        settings.id = id;
      }
      return updateSettings(settings);
    };

    fetchData().catch(console.error);
  };

  const refreshPIDData = () => {
    const fetchData = async () => {
      return getPIDData().then((data: PIDData[]) => {
        data.forEach((pidData: PIDData) => {
          if (props.selected && props.selected.id === pidData.id) {
            const { interval, rotations, errors } = pidData;
            setState({
              interval,
              rotations,
              errors,
              timePeriod: state.timePeriod,
            });
          }
        });
      });
    };

    fetchData().catch(console.error);
  };

  React.useEffect(() => {
    const fetchData = async () => {
      return getPIDData().then((data: PIDData[]) => {
        data.forEach((pidData: PIDData) => {
          if (props.selected && props.selected.id === pidData.id) {
            const { interval, rotations, errors } = pidData;
            setState({
              interval,
              rotations,
              errors,
              timePeriod: state.timePeriod,
            });
          }
        });
      });
    };

    fetchData().catch(console.error);
  }, []);

  const onlyLastNSeconds = (array: number[]): number[] => {
    const lastN = (state.timePeriod * 1000) / state.interval;

    if (lastN > array.length) {
      return array;
    } else {
      return array.slice(-lastN);
    }
  };

  return (
    <PIDTab
      onFormSubmit={onFormSubmit}
      interval={state.interval}
      rotations={onlyLastNSeconds(state.rotations)}
      errors={onlyLastNSeconds(state.errors)}
      refresh={refreshPIDData}
    />
  );
};

export default PIDTabController;
