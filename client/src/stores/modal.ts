import { acceptHMRUpdate, defineStore } from 'pinia';
import { ref } from 'vue';

export const useModalStore = defineStore('modal', () => {
  const isOpen = ref(false);
  const confirmed = ref(false);
  const title = ref('Confirm?');
  const description = ref('');
  const cancelButton = ref('Cancel');
  const confirmButton = ref('Confirm');

  function open(text: string) {
    confirmed.value = false;
    description.value = text;
    isOpen.value = true;
  }

  function close() {
    isOpen.value = false;
  }

  function confirm() {
    confirmed.value = true;
    console.log(confirmed.value);
    close();
  }

  return {
    isOpen,
    confirmed,
    title,
    description,
    cancelButton,
    confirmButton,
    open,
    confirm,
    close,
  };
});

if(import.meta.hot) {
  import.meta.hot.accept(acceptHMRUpdate(useModalStore, import.meta.hot));
}
