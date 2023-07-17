<script setup lang="ts">
import type { View } from '@/data/api';
import ChartGraph from './ChartGraph.vue';
import KeysList from './KeysList.vue';

interface Props {
  view?: View | null,
  busy: boolean,
  invalid: boolean,
}

const props = withDefaults(
  defineProps<Props>(), {
    view: null,
    busy: false,
    invalid: false,
  }
);

function getGridStyle() {
  let style: { [name: string]: string } = {};

  if(props.view?.data.templateColumns && props.view?.data.templateRows) {
    style['grid-template-columns'] = props.view.data.templateColumns;
    style['grid-template-rows'] = props.view.data.templateRows;
  }

  return style;
}
</script>

<template>
<main class="container-fluid">
  <div class="grid" :style="getGridStyle()">
    <KeysList />
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
