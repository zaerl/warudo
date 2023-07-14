<script setup lang='ts'>
import type { ECOption } from '@/charts';
import { ref } from 'vue';
import VChart from 'vue-echarts';

interface Props {
  title: string,
  legend: boolean,
}

const props = withDefaults(
  defineProps<Props>(), {
    title: 'Test',
    busy: false,
    invalid: false,
    legend: true,
  }
);

const option = ref<ECOption>({
  title: {
    text: props.title,
    left: 'center'
  },
  tooltip: {
    trigger: 'item',
    formatter: '{a} <br/>{b} : {c} ({d}%)'
  },
  legend: {
    show: props.legend,
    orient: 'vertical',
    left: 'left',
    data: ['Direct', 'Email', 'Ad Networks', 'Video Ads', 'Search Engines']
  },
  series: [
    {
      name: 'Traffic Sources',
      type: 'pie',
      radius: '55%',
      center: props.title !== '' || props.legend ? ['50%', '60%'] : ['50%', '50%'],
      data: [
        { value: 335, name: 'Direct' },
        { value: 310, name: 'Email' },
        { value: 234, name: 'Ad Networks' },
        { value: 135, name: 'Video Ads' },
        { value: 1548, name: 'Search Engines' }
      ],
      emphasis: {
        itemStyle: {
          shadowBlur: 10,
          shadowOffsetX: 0,
          shadowColor: 'rgba(0, 0, 0, 0.5)'
        }
      }
    }
  ]
});
</script>

<template>
<v-chart class='chart' :option='option' :loading='props.busy' autoresize />
</template>

<style scoped>
.chart {
  height: 400px;
}
</style>
