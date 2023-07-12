<script setup lang="ts">
import MainGrid from '@/components/MainGrid.vue'
import SearchBar from '@/components/SearchBar.vue'
import { useRoute } from 'vue-router';

const route = useRoute();
const id = parseInt(route.params.id as string);

</script>

<template>
<SearchBar />
<MainGrid />
</template>
