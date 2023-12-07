import { useState } from 'react'
import './App.css'
import {images} from './images'

function new_dicevalue() {
  return {
    "type": "mana",
    "value": 0
  }
}

function new_dice(){
  return {
    "isSeed": false,
    "age": 0,
    "sideCount": 0,
    "sideValues": [],
    "sideWeights": [],
    "maxLifespan": 0,
    "manaCost": 0
  }
}


function DiceSideImage({type}) {
  let imgsrc = images["dice_mana"]
  switch(type) {
    case "mana":
      imgsrc = images["dice_mana"]
      break
    case "fire":
      imgsrc = images["dice_fire"]
      break
    case "heart":
      imgsrc = images["dice_heart"]
      break
  }
  return (
    <img className="dicetype-icon" src={imgsrc}/>
  )
}
function Editor({dices, setDices, currentDiceIndex, setDiceIndex}) {
  return (
    <form id="Editor">
      <label className="dice-input">
        Is the dice a seed?
        <input 
          onChange={(event)=>{setDices(dices.map((dice, index) => 
              index == currentDiceIndex ? {...dice, isSeed: event.currentTarget.checked} : dice 
          ))}}
          checked={dices[currentDiceIndex].isSeed}
          type="checkbox"
          name="isSeed"
        />
      </label>
      <label className="dice-input">
        Age of Dice:
        <input 
          onChange={(event)=>{setDices(dices.map((dice, index) => 
            index == currentDiceIndex ? {...dice, age: event.currentTarget.value} : dice 
          ))}}
          value={dices[currentDiceIndex].age}
          type="range"
          name="age_range"
          min="0"
          max="100"
        />
        <input
          onChange={(event)=>{setDices(dices.map((dice, index) => 
            index == currentDiceIndex ? {...dice, age: event.currentTarget.value} : dice 
          ))}}
          value={dices[currentDiceIndex].age}
          type="number"
          name="age_input"
          min="0"
          max="100"
        />
      </label>
      <label className="dice-input">
        Number of sides of Dice:<br/>
        <label>
          4
          <input 
            onChange={(event)=>{setDices(dices.map((dice, index) => 
              {
                if(index != currentDiceIndex) return dice;
                const value = event.currentTarget.value;
                let newSideValues = [...dice.sideValues];
                let newSideWeights = [...dice.sideWeights];
                newSideValues.length = value
                newSideWeights.length = value
                for(let i = 0; i < value; i++) {
                  newSideValues[i] = new_dicevalue()
                  newSideWeights[i] = 1
                }
                return {...dice, sideCount: value, sideValues: newSideValues, sideWeights: newSideWeights} 
              }
            ))}}
            checked={dices[currentDiceIndex].sideCount == 4}
            value="4"
            type="radio"
            name="sideCount_4"
          />
        </label>
        <label>
          6
          <input 
            onChange={(event)=>{setDices(dices.map((dice, index) => 
              {
                if(index != currentDiceIndex) return dice;
                const value = event.currentTarget.value;
                let newSideValues = [...dice.sideValues];
                let newSideWeights = [...dice.sideWeights];
                newSideValues.length = value
                newSideWeights.length = value
                for(let i = 0; i < value; i++) {
                  newSideValues[i] = new_dicevalue()
                  newSideWeights[i] = 1
                }
                return {...dice, sideCount: value, sideValues: newSideValues, sideWeights: newSideWeights} 
              }
            ))}}
            checked={dices[currentDiceIndex].sideCount == 6}
            value="6"
            type="radio"
            name="sideCount_6"
          />
        </label>
      </label>
      <label className="dice-input">
        Max Lifespan of Dice:
        <input 
          onChange={(event)=>{setDices(dices.map((dice, index) => 
            index == currentDiceIndex ? {...dice, maxLifespan: event.currentTarget.value} : dice 
          ))}}
          value={dices[currentDiceIndex].maxLifespan}
          type="range"
          name="lifespan_range"
          min="0"
          max="100"
        />
        <input
          onChange={(event)=>{setDices(dices.map((dice, index) => 
            index == currentDiceIndex ? {...dice, maxLifespan: event.currentTarget.value} : dice 
          ))}}
          value={dices[currentDiceIndex].maxLifespan}
          type="number"
          name="lifespan_input"
          min="0"
          max="100"
        />
      </label>
      <label className="dice-input">
        Mana Cost of Dice:
        <input 
          onChange={(event)=>{setDices(dices.map((dice, index) => 
            index == currentDiceIndex ? {...dice, manaCost: event.currentTarget.value} : dice 
          ))}}
          value={dices[currentDiceIndex].manaCost}
          type="range"
          name="manaCost_range"
          min="0"
          max="100"
        />
        <input
          onChange={(event)=>{setDices(dices.map((dice, index) => 
            index == currentDiceIndex ? {...dice, manaCost: event.currentTarget.value} : dice 
          ))}}
          value={dices[currentDiceIndex].manaCost}
          type="number"
          name="manaCost_input"
          min="0"
          max="100"
        />
      </label>
      { dices[currentDiceIndex].sideCount != 0 &&
        <div className="dice-input sides-input">{
          Array.from({length: dices[currentDiceIndex].sideCount})
          .map((item, index) => {
            return <div key={index}>
              Side #{index}
              <select onChange={(event)=>{setDices(dices.map((dice, index_j) => 
              {
                if(index_j != currentDiceIndex) return dice;
                let newSideValues = [...dice.sideValues];
                newSideValues[index].type = event.currentTarget.value;
                return {...dice, sideValues: newSideValues} 
              }))}} value={dices[currentDiceIndex].sideValues[index].type} name="sideValue_type">
                <option value="mana">Mana</option>
                <option value="fire">Fire</option>
                <option value="heart">Heart</option>
              </select>
              <DiceSideImage type={dices[currentDiceIndex].sideValues[index].type}/>
              <label>
                Side Value
                <input 
                  onChange={(event)=>{setDices(dices.map((dice, index_j) => 
                  {
                    if(index_j != currentDiceIndex) return dice;
                    let newSideValues = [...dice.sideValues];
                    newSideValues[index].value = Number(event.currentTarget.value);
                    return {...dice, sideValues: newSideValues} 
                  }))}}
                  value={dices[currentDiceIndex].sideValues[index].value}
                  type="number"
                  name="sideValue_value"
                />
              </label>
              <label>
                Side Weights
                <input 
                  onChange={(event)=>{setDices(dices.map((dice, index_j) => 
                  {
                    if(index_j != currentDiceIndex) return dice;
                    let newSideWeights = [...dice.sideWeights];
                    newSideWeights[index] = Number(event.currentTarget.value);
                    return {...dice, sideWeights: newSideWeights} 
                  }))}}
                  value={dices[currentDiceIndex].sideWeights[index]}
                  type="number"
                  step="any"
                  name="sideValue_weight"
                />
              </label>
            </div>
          })
        }</div>
      }
    </form>
  );
}

function App() {
  const [dices, setDices] = useState([])
  const [currentDiceIndex, setDiceIndex] = useState(-1)

  function Create() {
    return <button onClick={() => {
      setDices(()=> {
        let newDices = [...dices,new_dice()];
        setDiceIndex(newDices.length - 1);
        return newDices
      });
    }}>
      Create Dice
    </button>
  }
  function Delete() {
    return <button onClick={() => {
      setDices(()=> {
        let newDices = dices.toSpliced(currentDiceIndex, 1);
        if(newDices.length == 0) {
          setDiceIndex(-1);
        }
        else if(currentDiceIndex >= newDices.length) {
          setDiceIndex(currentDiceIndex - 1)
        }
        return newDices
      });
    }}>
      Delete
    </button>
  }
  function Clear() {
    return <button onClick={() => {    
      setDices([]);
      setDiceIndex(-1);
    }}>
      Clear
    </button>
  }
  function Load() {
    return <button onClick={() => {    
      //  https://stackoverflow.com/a/40971885
      var input = document.createElement('input');
      input.type = 'file';
      input.onchange = e => { 
        var file = e.target.files[0]; 
        var reader = new FileReader();
        reader.readAsText(file,'UTF-8');
        reader.onload = readerEvent => {
          setDices(JSON.parse(readerEvent.target.result).dices)
          setDiceIndex(0)
        }
      }
      input.click();
      input.remove();
    }}>
      Load
    </button>
  }
  function Save() {
    return <button onClick={() => {
      const link = document.createElement("a");
      const file = new Blob([JSON.stringify({"dices": dices})], {
        type: "text/plain;charset=utf-8",
      });
      link.href = URL.createObjectURL(file);
      link.download = "dice.json";
      link.click();
      URL.revokeObjectURL(link.href);
      link.remove();
    }}>
      Save
    </button>
  }
  function Debug() {
    return <button style={{display: 'none'}  } onClick={() => {    
      console.log(dices[currentDiceIndex])
    }}>
      Debug
    </button>
  }

  function DiceList() {
    return (
    <div  id="DiceList">
      {
        dices.map((item,i) =>
          <div
            onClick={()=>{setDiceIndex(i)}} 
            className={`dice-item ${currentDiceIndex == i ? "selected" : ""}`} 
            key={i}
          >
            {`${item.sideCount} Sided Die`}
          </div>)
      }
  </div>)

  }
  return (
    <>
      <h1>Dice Editor</h1>
      <Create/><Delete/><Clear/><Load/><Save/><Debug/>
      <div id="EditorWindow">
        <DiceList/>{currentDiceIndex != -1 && 
          <Editor dices={dices} setDices={setDices}
            currentDiceIndex={currentDiceIndex} setDiceIndex={setDiceIndex}
          />}
      </div>
    </>
  )
}

export default App
