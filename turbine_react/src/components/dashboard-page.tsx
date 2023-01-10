import React from "react";
import { Turbine } from "../models/turbine-model";
import DashboardTable from "./dashboard-table";
import DashboardFormController from "../controllers/dashboard-form-controller";
import { Box, Paper } from "@mui/material";
import RefreshIcon from "@mui/icons-material/Refresh";
import IconButton from "@mui/material/IconButton";

type TurbineViewProps = {
  turbines: Turbine[];
  refresh: () => void;
};
const DashboardPage: React.FC<TurbineViewProps> = (props: TurbineViewProps) => {
  const { turbines, refresh } = props;
  return (
    <Box
      display="flex"
      justifyContent="center"
      alignItems="center"
      minHeight="100vh"
    >
      <Paper elevation={10} sx={{ p: 2 }}>
        <DashboardTable turbines={turbines}></DashboardTable>
        <IconButton color="primary" onClick={refresh}>
          <RefreshIcon />
        </IconButton>
        <DashboardFormController turbines={turbines} />
      </Paper>
    </Box>
  );
};
export default DashboardPage;
