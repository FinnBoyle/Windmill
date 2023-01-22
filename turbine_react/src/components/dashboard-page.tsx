import React from "react";
import { Turbine } from "../models/turbine-model";
import DashboardTable from "./dashboard-table";
import DashboardFormController from "../controllers/dashboard-form-controller";
import { Box, Paper, Grid, Button } from "@mui/material";
import RefreshIcon from "@mui/icons-material/Refresh";
import IconButton from "@mui/material/IconButton";
import DashboardLineChart from "./tabs/dashboard-line-graph";
import Tabs from "@mui/material/Tabs";
import Tab from "@mui/material/Tab";
import { TextField, MenuItem } from "@mui/material";
import MetricsTab from "./tabs/metrics-tab";
import StepperTab from "./tabs/stepper-tab";
import PIDTab from "./tabs/pid-tab";
import StepperTabController from "../controllers/tabs/stepper-tab-controller";
import PIDTabController from "../controllers/tabs/pid-tab-controller";

type TurbineViewProps = {
  turbines: Turbine[];
  selected: Turbine | undefined;
  onTurbineChange: (e: React.ChangeEvent<HTMLInputElement>) => void;

  refreshTurbines: () => void;
};
enum DashboardTab {
  METRICS = 1,
  STEPPER = 2,
  PID = 3,
}
const DashboardPage: React.FC<TurbineViewProps> = (props: TurbineViewProps) => {
  const { turbines, selected, onTurbineChange, refreshTurbines } = props;

  //tab handling states/events
  const [tab, setTab] = React.useState<DashboardTab>(DashboardTab.METRICS);
  const onTabChange = (event: React.SyntheticEvent, newTab: number) => {
    setTab(newTab);
  };

  return (
    <Paper
      elevation={10}
      sx={{
        position: "fixed",
        left: "20%",
        right: "20%",
        top: "5%",
        bottom: "5%",
        overflowY: "auto",
        height: "90%",
        px: 2,
        pb: 1,
      }}
    >
      <Box sx={{ mt: 2 }}>
        <Button variant="contained" sx={{ mr: 2 }} onClick={refreshTurbines}>
          Refresh <RefreshIcon />
        </Button>
        <TextField
          id="id"
          select
          label="ID"
          size="small"
          value={selected ? selected.id : undefined}
          onChange={(e: React.ChangeEvent<HTMLInputElement>) => onTurbineChange}
        >
          {turbines.map((turbine) => (
            <MenuItem key={turbine.id} value={turbine.id}>
              {turbine.id}
            </MenuItem>
          ))}
        </TextField>
      </Box>
      <Box sx={{ borderBottom: 1, borderColor: "divider" }}>
        <Tabs
          value={tab}
          onChange={onTabChange}
          aria-label="basic tabs example"
        >
          <Tab label="Metrics" value={DashboardTab.METRICS} />
          <Tab label="Stepper" value={DashboardTab.STEPPER} />
          <Tab label="PID" value={DashboardTab.PID} />
        </Tabs>
      </Box>

      {tab === DashboardTab.METRICS && <MetricsTab turbines={turbines} />}
      {tab === DashboardTab.STEPPER && (
        <StepperTabController selected={selected} />
      )}
      {tab === DashboardTab.PID && <PIDTabController selected={selected} />}
    </Paper>
  );
};
export default DashboardPage;
