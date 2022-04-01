import React from 'react';
import ReactDOM from 'react-dom';
import App from './components/App';
import {Provider} from "mobx-react";
import i18n from 'i18next';
import {initReactI18next} from 'react-i18next';
import {languagePacks} from "./static/strings";
import DataStore from "./store/data";
import SystemStore from "./store/system";
import Store from "./store/store";
import AnalysisStore from "./store/analysis";

const systemStore = new SystemStore();
const dataStore = new DataStore();
const analysisStore = new AnalysisStore();
Store.init();

i18n
  .use(initReactI18next)
  .init({
      resources: languagePacks,
      lng: 'en', // if you're using a language detector, do not define the lng option
      fallbackLng: 'cn',

      interpolation: {
          escapeValue: false, // react already safes from xss => https://www.i18next.com/translation-function/interpolation#unescape
      },
  });

ReactDOM.render(
  <React.StrictMode>
      <Provider data={dataStore} system={systemStore} analysis={analysisStore}>
          <App/>
      </Provider>
  </React.StrictMode>,
  document.getElementById('root')
);
