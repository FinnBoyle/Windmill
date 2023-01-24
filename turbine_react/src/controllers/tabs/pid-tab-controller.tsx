import PIDTab from "../../components/tabs/pid-tab";
import { Settings, Turbine, updateSettings } from "../../models/turbine-model";

type PIDTabProps = {
  selected: Turbine | undefined;
};
const PIDTabController: React.FC<PIDTabProps> = (props: PIDTabProps) => {
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
  return <PIDTab onFormSubmit={onFormSubmit} />;
};

export default PIDTabController;
