const {Api, JsonRpc, RpcError} = require('@plasma/plasmajs');
const networkConfig = require('../config/plasma.network.config');
const JsSignatureProvider = require('@plasma/plasmajs/dist/eosjs-jssig').default;
const fetch = require('node-fetch');
const {TextEncoder, TextDecoder} = require('util');
const loggerMaker = require('../config/logger.js');
const logger = loggerMaker.getLogger('plasma-oracles');


const rpc = new JsonRpc(networkConfig.httpEndpoint, {fetch});
const signatureProvider = new JsSignatureProvider([networkConfig.currenciesKey]);
const api = new Api({rpc, signatureProvider, textDecoder: new TextDecoder(), textEncoder: new TextEncoder()});


const updateCurrencies = async (rates, token) => {
  const ratesArray = [];
  for (const rate of rates.values()) {
    ratesArray.push({
      rate: rate.rate,
      precision: networkConfig.precision,
      pair: `${rate.from}P-${rate.to}P`,
    })
  }
  logger.debug('rates prepared for sending into contract', {ratesArray});

  return await api.transact({
    actions: [{
      account: networkConfig.currenciesContract,
      name: 'updaterates',
      authorization: [{
        actor: networkConfig.currenciesAccount,
        permission: 'active',
      }],
      data: {
        user: networkConfig.currenciesAccount,
        data: ratesArray,
        type: token,
        memo: Date.now()
      },
    }]
  }, {
    blocksBehind: 3,
    expireSeconds: 30,
  });
};

module.exports = {
  updateCurrencies
};