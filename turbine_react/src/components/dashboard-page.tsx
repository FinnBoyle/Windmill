import React from "react";
import { Turbine } from "../models/turbine-model";
import DashboardTable from "./dashboard-table";
import DashboardFormController from "../controllers/dashboard-form-controller";
import { Box, Paper } from "@mui/material";
type TurbineViewProps = {
  turbines: Turbine[];
};
const DashboardPage: React.FC<TurbineViewProps> = (props: TurbineViewProps) => {
  const { turbines } = props;
  return (
    <Box
      display="flex"
      justifyContent="center"
      alignItems="center"
      minHeight="100vh"
    >
      <Paper elevation={10} sx={{ p: 2 }}>
        <DashboardTable turbines={turbines}></DashboardTable>
        <DashboardFormController turbines={turbines} />
      </Paper>
    </Box>
  );
};
export default DashboardPage;
