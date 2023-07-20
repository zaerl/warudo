export type GetDataType = 'keys' | 'entries' | 'dashboards'

export type OrderBy = 'id' | 'created' | 'modified' | null;
export type Sort = 'asc' | 'desc' | null;

export interface SearchParams {
  key?: string;
  value?: string;
}

export interface OrderByParams {
  id?: number;
  limit?: number;
  orderby?: OrderBy;
  sort?: Sort;
  search?: string;
}

export type SearchBarParams = OrderByParams & SearchParams;

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
  modified: number;
  data: EntryData;
}

export interface Dashboard {
  id: number;
  created: number;
  modified: number;
  data: {
    name: string;
    templateColumns: string;
    templateRows: string;
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
  type: GetDataType,
  search: SearchBarParams = {}
): Promise<T | null> {
  const params = new URLSearchParams(search as unknown as Record<string, string>);

  try {
    const response = await fetch(`http://localhost:6252/app/${type}?${params}`);

    if (!response.ok) {
      throw new Error('Network response was not ok');
    }

    const data = await response.json();

    return data;
  } catch (error: unknown) {
    return null;
  }
}
