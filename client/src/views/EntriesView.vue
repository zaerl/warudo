<script setup lang="ts">
import DateTime from '@/components/DateTime.vue';
import JSONBlock from '@/components/JSONBlock.vue';
import SearchBar from '@/components/SearchBar.vue';
import TableRowState from '@/components/TableRowState.vue';
import { getData, type Entry } from '@/data/api';
import router from '@/router';
import { ref, watch } from 'vue';

let entries = ref<Entry[] | null>([]);
let busy = ref(true);
let invalid = ref(false);
let search = ref('');

async function fetchData() {
  busy.value = true;
  const query = search.value !== '' ? { key: 'any', value: search.value } : undefined;
  entries.value = await getData<Entry[]>('entries', query );
  busy.value = false;
}

watch(search, fetchData);

fetchData();
</script>

<template>
<SearchBar v-model="search" />
<main class="container-fluid">
  <table>
    <thead>
      <tr>
        <td>#</td>
        <td>Created</td>
        <td>Data</td>
      </tr>
    </thead>
    <tbody>
      <TableRowState :busy="busy" :invalid="invalid" :count="entries?.length ?? 0" :columns="3" />
      <tr v-for="entry in entries" :key="entry.id" @click="router.push({ name: 'entry', params: { id: entry.id } })">
        <td>{{ entry.id }}</td>
        <td class="date"><DateTime :timestamp="entry.created" /></td>
        <td>
          <!--<samp><span v-for="(value, key, index) in entry.data"><span v-if="index !== 0">, </span><strong>{{ key }}</strong>: {{ value }}</span></samp>-->
          <JSONBlock :json="entry.data" :inline="true" />
        </td>
      </tr>
    </tbody>
  </table>
</main>
</template>

<style scoped>
tr {
  cursor: pointer;
}

td.date {
  white-space: pre;
}
</style>
