<script setup lang="ts">
import type { GridPosition } from '@/data/api';
import { ref } from 'vue';
import ChartGraph from './ChartGraph.vue';
import KeysList from './KeysList.vue';

let busy = ref(false);

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
    <ChartGraph :loading="busy" />
    <div></div>
    <div></div>
  </div>
</main>
</template>

<style scoped>
.main-grid {
  display: grid;
  grid-template-columns: repeat(20, 1fr);
  grid-template-rows: repeat(20, 1fr);
  grid-column-gap: var(--grid-spacing-horizontal);
  grid-row-gap: var(--grid-spacing-vertical);
}
</style>
