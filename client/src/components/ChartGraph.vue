<script setup lang="ts">
import type { ECOption } from '@/charts';
import { getData, type Key } from '@/data/api';
import { onMounted, ref } from 'vue';
import VChart from 'vue-echarts';

interface Props {
  title: string,
  legend: boolean,
}

const props = withDefaults(
  defineProps<Props>(), {
    title: 'Test',
    legend: true,
  }
);

let busy = ref(true);
let invalid = ref(false);
let keys: Key[] | null = null;

const option = ref<ECOption>({
  title: {
    text: props.title,
    left: 'center'
  },
  tooltip: {
    trigger: 'item',
  },
  legend: {
    show: props.legend,
    orient: 'vertical',
    left: 'left',
  }
});

onMounted(async () => {
  keys = await getData<Key[]>('keys') as Key[];

  busy.value = false;
  invalid.value = keys === null;

  if(keys) {
    const series = [
      {
        name: props.title !== '' ? props.title : 'Series',
        type: 'pie',
        radius: '55%',
        center: props.title !== '' || props.legend ? ['50%', '60%'] : ['50%', '50%'],
        data: keys,
        emphasis: {
          itemStyle: {
            shadowBlur: 10,
            shadowOffsetX: 0,
            shadowColor: 'rgba(0, 0, 0, 0.5)'
          }
        }
      }
    ];

    const legendLabels = keys.map(key => key.name);

    option.value.series = series as any;
    (option.value.legend as any).data = legendLabels as any;
  }
});
</script>

<template>
<v-chart class="chart" :option="option" :loading="busy" autoresize />
</template>

<style scoped>
.chart {
  height: 400px;
}
</style>
