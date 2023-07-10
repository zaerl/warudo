<script setup lang="ts">
interface Props {
  name: string;
  links: string[];
}

const props = defineProps<Props>();

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
