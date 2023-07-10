<script setup lang="ts">
import type { GridPosition } from '@/data/api';
import Graph from './Graph.vue';
import KeysList from './KeysList.vue';

function getGridStyle(position: GridPosition) {
  return {
    'grid-area': `${position.rowStart} / ${position.columnStart} | ${position.rowEnd} / ${position.columnEnd}`
  };
}

</script>

<template>
<main class="container-fluid">
  <div class="main-grid">
    <KeysList />
    <Graph />
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
