<script setup lang="ts">
import type { GridPosition, View } from '@/data/api';
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

function getGridStyle(position?: GridPosition) {
  if (!position) {
    return {}
  }

  return {
    'grid-area': `${position.rowStart} / ${position.columnStart} | ${position.rowEnd} / ${position.columnEnd}`
  }
}
</script>

<template>
<main class="container-fluid">
  <div class="main-grid">
    <KeysList :style="getGridStyle()" />
    <div class="box">
      <ChartGraph />
    </div>
    <div></div>
    <div></div>
  </div>
</main>
</template>

<style scoped>
.main-grid {
  display: grid;
  grid-template-columns: repeat(2, 1fr);
  grid-template-rows: repeat(2, 1fr);
  grid-column-gap: var(--grid-spacing-horizontal);
  grid-row-gap: var(--grid-spacing-vertical);
}
</style>
