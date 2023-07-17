export type DocumentTheme = 'auto' | 'light' | 'dark';

export function getMainTheme(theme: DocumentTheme = 'auto'): DocumentTheme {
  if(theme === 'auto') {
    return window.matchMedia('(prefers-color-scheme: dark)').matches ? 'dark' : 'light';
  }

  return theme;
}

export function setMainTheme(theme: DocumentTheme = 'auto') {
  document.querySelector('html')?.setAttribute('data-theme', getMainTheme(theme));
}
