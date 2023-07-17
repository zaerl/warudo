<script setup lang="ts">
import { RouterLink } from 'vue-router';

interface Props {
  name: string
}

const props = defineProps<Props>();
</script>

<template>
<div id="navs" class="container-fluid">
  <nav>
    <ul>
      <li class="main-title">
        <RouterLink to="/">{{ props.name }}</RouterLink>
      </li>
    </ul>
    <ul>
      <li>
        <RouterLink to="/">List</RouterLink>
      </li>
      <li>
        <RouterLink to="/dashboards">Dashboards</RouterLink>
      </li>
      <li>
        <RouterLink to="/settings">Settings</RouterLink>
      </li>
    </ul>
  </nav>
</div>
</template>

<style scoped>
.main-title {
  font-weight: bold;
  cursor: pointer;
}
</style>
