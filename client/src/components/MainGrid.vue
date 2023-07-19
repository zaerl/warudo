<script setup lang="ts">
import type { Dashboard } from '@/data/api';
import ChartGraph from './ChartGraph.vue';
import KeysList from './KeysList.vue';

interface Props {
  dashboard?: Dashboard | null,
  busy?: boolean,
  invalid?: boolean,
}

const props = withDefaults(
  defineProps<Props>(), {
    dashboard: null,
    busy: false,
    invalid: false,
  }
);

function getGridStyle() {
  let style: { [name: string]: string } = {};

  if(props.dashboard?.data.templateColumns && props.dashboard?.data.templateRows) {
    style['grid-template-columns'] = props.dashboard.data.templateColumns;
    style['grid-template-rows'] = props.dashboard.data.templateRows;
  }

  return style;
}
</script>

<template>
<main class="container-fluid">
  <div class="grid" :style="getGridStyle()">
    <div>
      <KeysList :type="'keys'" />
    </div>
    <div class="box">
      <ChartGraph />
    </div>
  </div>
</main>
</template>

<style scoped>
/*.main-grid {
  display: grid;
  grid-template-columns: repeat(auto-fit, minmax(0%, 1fr));
  grid-column-gap: var(--grid-spacing-horizontal);
  grid-row-gap: var(--grid-spacing-vertical);
}*/
</style>
