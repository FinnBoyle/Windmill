import PIDTab from "../../components/tabs/pid-tab";
import {
  getPIDData,
  PIDData,
  Settings,
  Turbine,
  updateSettings,
} from "../../models/turbine-model";
import * as React from "react";
import io from "socket.io-client";

const socket = io("http://localhost:5001", {
  transports: ["websocket", "polling", "flashsocket"],
});

type PIDTabProps = {
  selected: Turbine | undefined;
};
type PIDTabState = {
  interval: number;
  rotations: number[];
  errors: number[];
};
const PIDTabController: React.FC<PIDTabProps> = (props: PIDTabProps) => {
  const [isConnected, setIsConnected] = React.useState(socket.connected);
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
    // const seconds = parseInt((form.seconds as HTMLInputElement).value);

    // if (isNaN(kp) && isNaN(ki) && isNaN(kd)) {
    //   setState({
    //     ...state,
    //     timePeriod: seconds,
    //   });
    // }

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
            if (pidData.errors.length > 200) {
              setState({
                interval,
                rotations: rotations.slice(rotations.length - 201, undefined),
                errors: errors.slice(errors.length - 201, undefined),
              });
            } else {
              setState({
                interval,
                rotations,
                errors,
              });
            }
          }
        });
      });
    };

    fetchData().catch(console.error);
  });

  React.useEffect(() => {
    socket.on("connect", () => {
      setIsConnected(true);
    });

    socket.on("disconnect", () => {
      setIsConnected(false);
    });

    socket.on("pidFeed", (data: PIDData) => {
      if (data.interval === state.interval) {
        if (state.errors.length >= 200) {
          //remove the oldest N items before pushing the new N items
          state.errors = state.errors.slice(data.errors.length - 1, undefined);
          state.rotations = state.rotations.slice(
            data.rotations.length - 1,
            undefined
          );
        }
        state.errors.push(...data.errors);
        state.rotations.push(...data.rotations);
        console.log(state.errors);
        setState({
          interval: state.interval,
          rotations: [...state.rotations, ...data.rotations],
          errors: [...state.errors, ...data.errors],
        });
      } else {
      }
    });

    return () => {
      socket.off("connect");
      socket.off("disconnect");
      socket.off("pong");
    };
  }, []);

  // const onlyLastNSeconds = (array: number[]): number[] => {
  //   const lastN = (state.timePeriod * 1000) / state.interval;

  //   if (lastN > array.length) {
  //     return array;
  //   } else {
  //     return array.slice(-lastN);
  //   }
  // };

  return (
    <PIDTab
      onFormSubmit={onFormSubmit}
      interval={state.interval}
      rotations={state.rotations}
      errors={state.errors}
      refresh={refreshPIDData}
      connected={isConnected}
    />
  );
};

export default PIDTabController;
