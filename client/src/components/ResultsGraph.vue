<script setup lang="ts">
import { ref } from 'vue';

let ariaBusy = ref(true);
</script>

<template>
<div id="main-graph-box" :aria-busy="ariaBusy" class="box">
  <canvas id="main"></canvas>
</div>
</template>

<style scoped></style>
