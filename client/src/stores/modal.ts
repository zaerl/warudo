import { acceptHMRUpdate, defineStore } from 'pinia';
import { ref } from 'vue';

export const useModalStore = defineStore('modal', () => {
  const isOpen = ref(false);
  const title = ref('Confirm?');
  const description = ref('');
  const cancelButton = ref('Cancel');
  const confirmButton = ref('Confirm');

  function open(text: string) {
    description.value = text;
    isOpen.value = true;
  }

  function close() {
    isOpen.value = false;
  }

  return {
    isOpen,
    title,
    description,
    cancelButton,
    confirmButton,
    open,
    close,
  };
});

if(import.meta.hot) {
  import.meta.hot.accept(acceptHMRUpdate(useModalStore, import.meta.hot));
}
