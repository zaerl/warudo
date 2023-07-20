import type { OrderBy, SearchBarParams, SearchParams, Sort } from "./api";

export function getSearch(search: string): SearchParams | null {
  if(typeof search == 'string' && search.length > 0) {
    const split = search.split(/\s+/);

    if(split.length >= 2 && split[0].length > 0 && split[1].length > 0) {
      const key = split[0].startsWith('$.') ? split[0] : '$.' + split[0];
      split.shift();

      const value = split.join(' ');

      return { key, value };
    }
  }

  return null;
}

export function getQuery(search: string, query: SearchBarParams) {
  const ret: SearchBarParams = {};
  const searchParams = getSearch(search);

  if(typeof query === 'undefined') {
    return ret;
  }

  if(typeof query.sort !== 'undefined' && query.sort !== null) {
    ret.sort = query.sort as Sort;
  }

  if(typeof query.orderby !== 'undefined' && query.orderby !== null) {
    ret.orderby = query.orderby as OrderBy;
  }

  return { ...ret, ...searchParams };
}

export function defaultHeaders(key = 'data', name = 'Data') {
  return [
    {
      key: 'id',
      name: '#',
    },
    {
      key: 'created',
      name: 'Created',
    },
    {
      key,
      name,
    },
  ];
}
