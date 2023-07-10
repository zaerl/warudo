<script setup lang="ts">
import Graph from './components/Graph.vue';
import KeysList from './components/KeysList.vue';
</script>

<template>
  <nav class="container-fluid">
    <ul>
      <li><strong>Warudo Dashboard</strong></li>
    </ul>
  </nav>
  <main class="container-fluid">
    <div class="grid">
      <KeysList />
      <Graph />
    </div>
  </main>
  <footer></footer>
</template>

<style scoped>

</style>
