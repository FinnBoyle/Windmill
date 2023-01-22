import DashboardForm from "../components/dashboard-form";
import React from "react";
import { Settings, Turbine, updateSettings } from "../models/turbine-model";

type DashboardFormProps = {
  turbines: Turbine[];
};

type DashboardFormState = {
  id: string;
  stepperState: number;
};

const DashboardFormController: React.FC<DashboardFormProps> = (
  props: DashboardFormProps
) => {
  const { turbines } = props;
  const [state, setState] = React.useState<DashboardFormState>({
    id: "",
    stepperState: 0,
  });

  const onFormSubmit = (e: React.FormEvent<HTMLFormElement>): void => {
    e.preventDefault();
    const form = e.target as HTMLFormElement;
    const rpm = parseInt((form.rpm as HTMLInputElement).value);
    const steps = parseInt((form.steps as HTMLInputElement).value);
    const kp = parseInt((form.kp as HTMLInputElement).value);
    const ki = parseInt((form.ki as HTMLInputElement).value);
    const kd = parseInt((form.kd as HTMLInputElement).value);

    const fetchData = async () => {
      let settings: Settings = {
        id: state.id,
        stepperState: state.stepperState,
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
      if (!isNaN(steps)) {
        settings.steps = steps;
      }
      if (!isNaN(rpm)) {
        settings.rpm = rpm;
      }
      if (state.id !== "") {
        settings.id = state.id;
      }
      return updateSettings(settings);
    };

    fetchData().catch(console.error);
  };

  const onChange = (e: React.ChangeEvent<HTMLInputElement>, target: string) => {
    if (target === "ID") {
      setState({ ...state, id: e.target.value });
    } else if (target === "STEPPER_STATE") {
      setState({ ...state, stepperState: parseInt(e.target.value) });
    }
    return;
  };

  return (
    <DashboardForm
      onSubmit={onFormSubmit}
      onChange={onChange}
      turbines={turbines}
      selectedId={state.id}
    />
  );
};

export default DashboardFormController;
