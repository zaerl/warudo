<script setup lang="ts">
import { useModalStore } from '@/stores/modal';

const modalStore = useModalStore();

function confirm() {
  modalStore.close();
}
console.log(modalStore);
</script>

<template>
<dialog :open="modalStore.isOpen">
  <article>
    <header>
        <a aria-label="Close" class="close" @click="modalStore.close"></a>
        {{ modalStore.title }}
    </header>
    <p>{{ modalStore.description }}</p>
    <footer>
      <a
        href=""
        role="button"
        class="secondary"
        @click.prevent="modalStore.close">
        {{ modalStore.cancelButton }}
      </a>
      <a
        href=""
        role="button"
        @click.prevent="confirm">
        {{ modalStore.confirmButton }}
      </a>
    </footer>
  </article>
</dialog>
</template>

<style scoped></style>
