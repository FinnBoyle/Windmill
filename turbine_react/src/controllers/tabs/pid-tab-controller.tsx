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
};
const PIDTabController: React.FC<PIDTabProps> = (props: PIDTabProps) => {
  const [state, setState] = React.useState<PIDTabState>({
    interval: 0,
    rotations: [],
    errors: [],
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

  React.useEffect(() => {
    const fetchData = async () => {
      return getPIDData().then((data: PIDData[]) => {
        data.forEach((pidData: PIDData) => {
          if (props.selected && props.selected.id === pidData.id) {
            const { interval, rotations, errors } = pidData;
            setState({ interval, rotations, errors });
          }
        });
      });
    };

    fetchData().catch(console.error);
  }, []);
  return (
    <PIDTab
      onFormSubmit={onFormSubmit}
      interval={state.interval}
      rotations={state.rotations}
      errors={state.errors}
    />
  );
};

export default PIDTabController;
