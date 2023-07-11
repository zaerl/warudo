export type DocumentTheme = 'auto' | 'light' | 'dark';

export function setMainTheme(theme: DocumentTheme = 'auto') {
  if(theme === 'auto') {
    theme = window.matchMedia('(prefers-color-scheme: dark)').matches ? 'dark' : 'light';
  }

  document.querySelector('html')?.setAttribute('data-theme', theme);
}
