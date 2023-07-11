<script setup lang="ts">
import { RouterLink } from 'vue-router'

interface Props {
  name: string
}

const props = defineProps<Props>()
</script>
<template>
<div id="navs" class="container-fluid">
  <nav>
    <ul>
      <li>
        <strong>{{ name }}</strong>
      </li>
    </ul>
    <ul>
      <li>
        <RouterLink to="/settings">Settings</RouterLink>
      </li>
    </ul>
  </nav>
</div>
</template>
