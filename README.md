# Formality

[![npm version](https://img.shields.io/npm/v/re-formality.svg?style=flat-square)](https://www.npmjs.com/package/re-formality)
[![build status](https://img.shields.io/travis/alexfedoseev/re-formality/master.svg?style=flat-square)](https://travis-ci.org/alexfedoseev/re-formality)
[![dependencies status](https://img.shields.io/gemnasium/alexfedoseev/re-formality.svg?style=flat-square)](https://gemnasium.com/alexfedoseev/re-formality)
[![license](https://img.shields.io/npm/l/re-formality.svg?style=flat-square)](https://www.npmjs.com/package/re-formality)

Reasonable form validation tool for [`reason-react`](https://reasonml.github.io/reason-react/).

## Features
* Validation strategies
* Dependant fields validation
* Async validations (on blur / debounced on change)
* I18n compat

## Examples

* Signup form [ [live](https://formality.now.sh/#signup) &middot; [source](examples/SignupForm.re) ]
* Login form [ [live](https://formality.now.sh/#login) &middot; [source](examples/LoginForm.re) ]

## Installation

```shell
# yarn
yarn add re-formality
# or npm
npm install --save re-formality
```

## 🚧 Docs are WIP 🚧

## Usage

Here's the quick example. See [`examples`](examples/) for real-world cases.

```reason
module MyForm = {
  type field =
    | Email
    | Password;
  type value = string;
  type state = {
    email: string,
    password: string
  };
  type message = string;
  let get = (field, state) =>
    switch field {
    | Email => state.email
    | Password => state.password
    };
  let update = ((field, value), state) =>
    switch (field, value) {
    | (Email, value) => {...state, email: value}
    | (Password, value) => {...state, password: value}
    };
  let valueEmpty = value => value === "";
  let strategy = Formality.Strategy.OnFirstSuccessOrFirstBlur;
  module Validators = Formality.MakeValidators({type t = field;});
  type validators = Validators.t(Formality.validator(field, value, state, message));
  let validators = Formality.(
    Validators.empty
    |> Validators.add(Email, {
         strategy,
         dependents: None, /* You can define fields which must be revalidated on change of this field's value */
         validate: (value, _state) =>
           switch value {
           | "" => Invalid("Uh oh error")
           | _ => Valid
           }
       })
    |> Validators.add(Password, {
         strategy,
         dependents: None, /* You can define fields which must be revalidated on change of this field's value */
         validate: (value, _state) =>
           switch value {
           | "" => Invalid("Uh oh error")
           | _ => Valid
           }
       })
    );
};

module FormContainer = Formality.Make(MyForm);

let component = "MyForm" |> ReasonReact.statelessComponent;

let make = (_) => {
  ...component,
  render: (_) =>
    <FormContainer
      initialState={email: "", password: ""}
      onSubmit=((state, notify) => {
        /* Submit form and either notify.onSuccess / notify.onFailure */
      })
    >
      ...(
            form =>
              <form
                className="form"
                onSubmit=(form.submit |> Formality.Dom.preventDefault)>
                <input
                  value=form.state.email
                  disabled=(form.submitting |> Js.Boolean.to_js_boolean)
                  onChange=(
                    event =>
                      event
                      |> Formality.Dom.toValueOnChange
                      |> form.change(MyForm.Email)
                  )
                  onBlur=(
                    event =>
                      event
                      |> Formality.Dom.toValueOnBlur
                      |> form.blur(MyForm.Email)
                  )
                />
                (
                  switch (MyForm.Email |> form.results) {
                  | Some(Invalid(message)) =>
                    <div className=(Cn.make(["form-message", "failure"]))>
                      (message |> ReasonReact.stringToElement)
                    </div>
                  | Some(Valid)
                  | None => ReasonReact.nullElement
                  }
                )
                <input
                  value=form.state.password
                  disabled=(form.submitting |> Js.Boolean.to_js_boolean)
                  onChange=(
                    event =>
                      event
                      |> Formality.Dom.toValueOnChange
                      |> form.change(MyForm.Password)
                  )
                  onBlur=(
                    event =>
                      event
                      |> Formality.Dom.toValueOnBlur
                      |> form.blur(MyForm.Password)
                  )
                />
                (
                  switch (MyForm.Password |> form.results) {
                  | Some(Invalid(message)) =>
                    <div className=(Cn.make(["form-message", "failure"]))>
                      (message |> ReasonReact.stringToElement)
                    </div>
                  | Some(Valid)
                  | None => ReasonReact.nullElement
                  }
                )
                <button disabled=(form.submitting |> Js.Boolean.to_js_boolean)>
                  ((form.submitting ? "Submitting..." : "Submit") |> ReasonReact.stringToElement)
                </button>
              </form>
          )
    </FormContainer>
};
```

## API

...

### Strategies
In most cases validation feedback should be provided as soon as possible, but not too soon. The question comes down to when to start providing feedback. It really depends on context. Using strategies below, the form won't provide any feedback until the specific moment, e.g. the first blur from the field or the first successful validation. All you have to do is to pick the most suitable one for your context. To understand the behavior of each strategy, add the following prefix to its name: "Start providing feedback on..."

```reason
module Strategy = {
  type t =
    | OnFirstBlur
    | OnFirstChange
    | OnFirstSuccess
    | OnFirstSuccessOrFirstBlur
    | OnSubmit;
};
```

#### `OnFirstBlur`
Results are emitted on the first blur. After first results are emitted, feedback is provided on every change in this field.

#### `OnFirstChange`
Results are emitted as user types in the field.

#### `OnFirstSuccess`
Results are emitted on first successful validation. After first results are emitted, feedback is provided on every change in this field.

#### `OnFirstSuccessOrFirstBlur` ✨
Results are emitted immediately on successful validation or on the first blur. After first results are emitted, feedback is provided on every change in this field.

#### `OnSubmit`
Results are emitted only after first submission attempt. After this, results for each field are emitted on every change until the form is reset or remounted.

...
