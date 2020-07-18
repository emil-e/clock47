let properties = {};

function path(p) {
  //return "http://clock47.local/" + p;
  return p;
}

function InputFrame(props) {
  return (
    <div className="input-field">
      <h2>{props.name}</h2>
      {props.children}
    </div>
  );
}

function ColorInput(props) {
  return (
    <InputFrame name={props.name}>
      <input type="color"
             value={"#" + props.value}
             onChange={event => props.onChange(event.target.value.substring(1))}
             className="color-input" />
    </InputFrame>
  );
}

function TextInput(props) {
  return (
    <InputFrame name={props.name}>
      <input type="text"
             value={props.value}
             onChange={event => props.onChange(event.target.value)}
             className="text-input" />
    </InputFrame>
  )
}

function SelectInput(props) {
  const options = props.options.split(",").map(
    opt => (<option value={opt} key={opt}>{opt}</option>));
  return (
    <InputFrame name={props.name}>
      <select value={props.value}
              onChange={event => props.onChange(event.target.value)}
              className="select-input">
        {options}
      </select>
    </InputFrame>
  )
}

function updateProperty(key, value) {
  properties[key] = value;
  update();
  let request = new Request(path("properties/" + key), {
    method: "PUT",
    body: value
  });
  return fetch(request);
}

function makeField(ElementType, name, key) {
  return (
    <ElementType value={properties[key]}
                 options={properties[key + "Options"]}
                 onChange={value => updateProperty(key, value)}
                 name={name}/>
  );
}

function root() {
  return (
    <div>
      <h1>Clock47</h1>
      {makeField(ColorInput, "Primary color", "color0")}
      {makeField(ColorInput, "Secondary color", "color1")}
      {makeField(TextInput, "Message", "message")}
      {makeField(SelectInput, "Mode", "mode")}
    </div>
  );
}

function update() {
  ReactDOM.render(root(), document.getElementById('root'));
}

fetch(path("properties"))
  .then(response => response.json())
  .then(props => {
    properties = props;
    update();
  });
