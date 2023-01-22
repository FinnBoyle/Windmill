import { Turbine } from "../../models/turbine-model";
import DashboardTable from "../dashboard-table";
import IconButton from "@mui/material/IconButton";
import RefreshIcon from "@mui/icons-material/Refresh";

type MetricsTabProps = {
  turbines: Turbine[];
};
const MetricsTab: React.FC<MetricsTabProps> = (props: MetricsTabProps) => {
  const { turbines } = props;
  return (
    <div>
      <DashboardTable turbines={turbines}></DashboardTable>
    </div>
  );
};

export default MetricsTab;
