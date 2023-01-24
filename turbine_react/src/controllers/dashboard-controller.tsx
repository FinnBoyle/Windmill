import { Turbine, getTurbines } from "../models/turbine-model";
import DashboardPage from "../components/dashboard-page";
import * as React from "react";

const DashboardController = () => {
  const [turbines, setTurbines] = React.useState<Turbine[]>([]);
  const [selected, selectTurbine] = React.useState<Turbine | undefined>();
  const refreshTurbines = () => {
    const fetchData = async () => {
      return getTurbines().then((turbines) => {
        setTurbines(turbines);
      });
    };

    fetchData().catch(console.error);
  };
  React.useEffect(() => {
    const fetchData = async () => {
      return getTurbines().then((turbines) => {
        setTurbines(turbines);
      });
    };

    fetchData().catch(console.error);
  }, []);

  const onTurbineChange = (e: React.ChangeEvent<HTMLInputElement>) => {
    turbines.forEach((turbine) => {
      console.log({ turbine: turbine.id, target: e.target.value });
      if (turbine.id === e.target.value) {
        console.log(turbine);
        selectTurbine(turbine);
      }
    });
  };

  return (
    <DashboardPage
      turbines={turbines}
      selected={selected}
      onTurbineChange={onTurbineChange}
      refreshTurbines={refreshTurbines}
    />
  );
};

export default DashboardController;
