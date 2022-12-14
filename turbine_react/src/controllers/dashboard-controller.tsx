import { Turbine, getTurbines } from "../models/turbine-model";
import DashboardPage from "../components/dashboard-page";
import * as React from "react";

const DashboardController = () => {
  const [turbines, setTurbines] = React.useState<Turbine[]>([]);

  React.useEffect(() => {
    const fetchData = async () => {
      return getTurbines().then((turbines) => {
        setTurbines(turbines);
      });
    };

    fetchData().catch(console.error);
  }, []);

  return <DashboardPage turbines={turbines} />;
};

export default DashboardController;
