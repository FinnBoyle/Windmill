import * as React from "react";
import Table from "@mui/material/Table";
import TableBody from "@mui/material/TableBody";
import TableCell from "@mui/material/TableCell";
import TableContainer from "@mui/material/TableContainer";
import TableHead from "@mui/material/TableHead";
import TableRow from "@mui/material/TableRow";
import Paper from "@mui/material/Paper";
import { Turbine } from "../../models/turbine-model";

type DashboardTableProps = {
  turbines: Turbine[];
};
const DashboardTable: React.FC<DashboardTableProps> = (
  props: DashboardTableProps
) => {
  const { turbines } = props;

  return (
    <TableContainer component={Paper}>
      <Table sx={{ minWidth: 650 }} aria-label="simple table">
        <TableHead>
          <TableRow>
            <TableCell>ID</TableCell>
            <TableCell>Memory</TableCell>
            <TableCell>Voltage</TableCell>
            <TableCell>Stepper State</TableCell>
            <TableCell>RPM</TableCell>
            <TableCell>rotation</TableCell>

            <TableCell>Kp</TableCell>
            <TableCell>Ki</TableCell>
            <TableCell>Kd</TableCell>
          </TableRow>
        </TableHead>
        <TableBody>
          {turbines.map((turbine, index) => {
            return (
              <TableRow
                key={index}
                sx={{ "&:last-child td, &:last-child th": { border: 0 } }}
              >
                <TableCell component="th" scope="row">
                  {turbine.id}
                </TableCell>
                <TableCell>{turbine.memory}</TableCell>

                <TableCell>{turbine.voltage}</TableCell>

                <TableCell>{turbine.stepperState}</TableCell>
                <TableCell>{turbine.rpm}</TableCell>
                <TableCell>{turbine.rotation}</TableCell>

                <TableCell>{turbine.kp}</TableCell>
                <TableCell>{turbine.ki}</TableCell>
                <TableCell>{turbine.kd}</TableCell>
              </TableRow>
            );
          })}
        </TableBody>
      </Table>
    </TableContainer>
  );
};

export default DashboardTable;
