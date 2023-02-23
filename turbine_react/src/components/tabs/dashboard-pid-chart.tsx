import { Line } from "react-chartjs-2";
import { Box } from "@mui/material";
import {
  Chart as ChartJS,
  CategoryScale,
  LinearScale,
  PointElement,
  LineElement,
  Title,
  Tooltip,
  Legend,
} from "chart.js";
// import zoomPlugin from "chartjs-plugin-zoom";

ChartJS.register(
  CategoryScale,
  LinearScale,
  PointElement,
  LineElement,
  Title,
  Tooltip,
  Legend
  // zoomPlugin
);
ChartJS.defaults.color = "#FFFFFF";

type PIDChartProps = {
  interval: number;
  rotations: number[];
  errors: number[];
};
const DashboardPIDChart: React.FC<PIDChartProps> = (props: PIDChartProps) => {
  const { interval, rotations, errors } = props;
  const data = {
    labels: Array.from(Array(rotations.length).keys()).map(
      (key) => key * interval
    ),
    datasets: [
      {
        label: "Turbine Rotation",
        data: rotations,
        borderWidth: 5,
        borderColor: "rgb(55,162,235)",
        backgroundColor: "rgb(53,162,235)",
        yAxisID: "y",
        pointRadius: 0.1,
        showLine: false,
      },
      {
        label: "PID Error",
        data: errors,
        borderWidth: 5,
        borderColor: "rgb(255,114,111)",
        backgroundColor: "rgb(255,114,111)",
        yAxisID: "y1",
        pointRadius: 0.1,
        showLine: false,
      },
    ],
  };

  const options = {
    responsive: true,
    animation: {
      duration: 0,
    },
    title: {
      display: true,
      text: "Turbine Rotation (°) vs Time (ms)",
    },
    scales: {
      y: {
        type: "linear" as const,
        display: true,
        position: "left" as const,
        min: 0,
        max: 360,
      },

      y1: {
        type: "linear" as const,
        display: true,
        position: "right" as const,
        grid: {
          drawOnChartArea: false,
        },
      },
    },
  };

  return <Line options={options} data={data}></Line>;
};

export default DashboardPIDChart;
