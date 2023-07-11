<script setup lang="ts">
import { ref } from 'vue';

let search = ref('');
</script>

<template>
<div id="search" class="container-fluid">
  <input type="search" id="search" name="search" placeholder="Search" v-model="search" />
</div>
</template>
