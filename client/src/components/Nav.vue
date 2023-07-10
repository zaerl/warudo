<script setup lang="ts">
defineProps({
  name: String,
  links: Array,
});
</script>
<template>
<div id="navs" class="container-fluid">
  <nav>
    <ul>
      <li><strong>{{ name }}</strong></li>
    </ul>
    <ul>
      <li v-for="link in links">
        <a href="#">{{ link }}</a>
      </li>
    </ul>
  </nav>
</div>
</template>
