export type GetDataType = 'keys' | 'entries' | 'views'

export interface SearchBarParams {
  id?: number;
  limit?: number;
  key?: string;
  value?: string;
}

export enum ViewItemTypes {
  List,
  Keys,
}

export interface GridPosition {
  rowStart: number | string;
  columnStart: number | string;
  rowEnd: number | string;
  columnEnd: number | string;
}

export interface ViewItem extends GridPosition {
  type: ViewItemTypes,
}

interface EntryData {
  [key: string]: number
}

export interface Entry {
  id: number;
  created: number;
  data: EntryData;
}

export interface View {
  id: number;
  created: number;
  modified: number;
  data: {
    name: string;
    rows: number;
    columns: number;
    items: ViewItem[];
  };
}

export interface Key {
  name: string,
  value: number,
}

export async function getData<T>(
  type: GetDataType = 'entries',
  search: SearchBarParams = {}
): Promise<T | null> {
  const params = new URLSearchParams(search as unknown as Record<string, string>);

  try {
    const response = await fetch(`http://localhost:6252/app/${type}?${params}`);
    const data = await response.json();

    return data;
  } catch (error: unknown) {
    if (error instanceof Error) {
      console.log(error.message);
    } else {
      console.log(error);
    }

    return null;
  }
}
