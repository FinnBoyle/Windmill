import { Line } from "react-chartjs-2";
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

ChartJS.register(
  CategoryScale,
  LinearScale,
  PointElement,
  LineElement,
  Title,
  Tooltip,
  Legend
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
        borderWidth: 1,
        borderColor: "rgb(55,162,235)",
        backgroundColor: "rgb(53,162,235)",
        yAxisID: "y",
      },
      {
        label: "PID Error",
        data: errors,
        borderWidth: 1,
        borderColor: "rgb(55,162,235)",
        backgroundColor: "rgb(53,162,235)",
        yAxisID: "y1",
      },
    ],
  };

  const options = {
    responsive: true,

    title: {
      display: true,
      text: "Turbine Rotation (°) vs Time (ms)",
    },
    scales: {
      y: {
        type: "linear" as const,
        display: true,
        position: "left" as const,
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
