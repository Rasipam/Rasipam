const stores = {}

export default class Store {
    static register(name, inst) {
        stores[name] = inst;
    }

    static init() {
        return Promise.all(
            Object.values(stores)
                .map(store => store.init())
        );
    }

    static getStores() {
        return stores;
    }
}
