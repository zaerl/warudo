import type { ComposeOption, PieSeriesOption, TitleComponentOption } from 'echarts';
import { PieChart } from 'echarts/charts';
import type { LegendComponentOption, TooltipComponentOption } from 'echarts/components';
import { LegendComponent, TitleComponent, TooltipComponent } from 'echarts/components';
import { use } from 'echarts/core';
import { CanvasRenderer } from 'echarts/renderers';

export type ECOption = ComposeOption<
  LegendComponentOption |
  PieSeriesOption |
  TitleComponentOption |
  TooltipComponentOption
>;

export function initCharts() {
  use([
    CanvasRenderer,
    LegendComponent,
    PieChart,
    TitleComponent,
    TooltipComponent,
  ]);
}

// provide(THEME_KEY, 'dark');
