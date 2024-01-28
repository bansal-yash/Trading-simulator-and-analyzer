var integerArray = [0, 1, 2, 3];
var integerArray2 = [0, 4, 5, 6, 7];
var integerArray3 = [0, 8, 9, 10, 11, 12, 13, 14];

function submitForm() {
  document.getElementById("stocks").submit();
  document.getElementById("options").submit();
  document.getElementById("time").submit();
  document.getElementById("stock 1").submit();
  document.getElementById("stock 2").submit();
}

function toggleMode(mode) {
  if (mode === 1) {
    document.getElementById("mode1Content").style.display = "block";
    document.getElementById("mode2Content").style.display = "none";
    document.getElementById("mode3Content").style.display = "none";
    document.getElementById("select_multiple_stocks").style.display = "none";
  } else if (mode === 2) {
    document.getElementById("mode1Content").style.display = "none";
    document.getElementById("mode2Content").style.display = "block";
    document.getElementById("mode3Content").style.display = "none";
    document.getElementById("select_multiple_stocks").style.display = "none";
  } else {
    document.getElementById("mode1Content").style.display = "none";
    document.getElementById("mode2Content").style.display = "none";
    document.getElementById("mode3Content").style.display = "block";
    document.getElementById("select_multiple_stocks").style.display = "grid";
  }
}

var x, i, j, l, ll, selElmnt, a, b, c;
/* Look for any elements with the class "custom-select": */
x = document.getElementsByClassName("custom-select");
l = x.length;
for (i = 0; i < l; i++) {
  selElmnt = x[i].getElementsByTagName("select")[0];
  ll = selElmnt.length;
  /* For each element, create a new DIV that will act as the selected item: */
  a = document.createElement("DIV");
  a.setAttribute("class", "select-selected");
  a.innerHTML = selElmnt.options[selElmnt.selectedIndex].innerHTML;
  x[i].appendChild(a);
  /* For each element, create a new DIV that will contain the option list: */
  b = document.createElement("DIV");
  b.setAttribute("class", "select-items select-hide");
  for (j = 0; j < ll; j++) {
    /* For each option in the original select element,
    create a new DIV that will act as an option item: */
    c = document.createElement("DIV");
    c.innerHTML = selElmnt.options[j].innerHTML;
    c.addEventListener("click", function (e) {
      /* When an item is clicked, update the original select box,
        and the selected item: */
      var y, i, k, s, h, sl, yl;
      s = this.parentNode.parentNode.getElementsByTagName("select")[0];
      sl = s.length;
      h = this.parentNode.previousSibling;
      for (i = 0; i < sl; i++) {
        if (s.options[i].innerHTML == this.innerHTML) {
          if (this.innerHTML == "Single Stock") {
            toggleMode(1);
          } else if (this.innerHTML == "Compare Stock") {
            toggleMode(2);
          } else if (this.innerHTML == "Multiple Stock") {
            toggleMode(3);
          }
          s.selectedIndex = i;
          h.innerHTML = this.innerHTML;
          y = this.parentNode.getElementsByClassName("same-as-selected");
          yl = y.length;
          for (k = 0; k < yl; k++) {
            y[k].removeAttribute("class");
          }
          this.setAttribute("class", "same-as-selected");
          var selectedElements = document.querySelectorAll(".select-selected");

          // Iterate over the NodeList (if needed)
          // console.log(selectedElements.length);
          if (selectedElements[0].innerHTML === "Single Stock") {
            for (var i = 0; i < integerArray.length; i++) {
              console.log(selectedElements[integerArray[i]].innerHTML);
            }
          } else if (selectedElements[0].innerHTML === "Compare Stock") {
            for (var i = 0; i < integerArray2.length; i++) {
              console.log(selectedElements[integerArray2[i]].innerHTML);
            }
          } else {
            for (var i = 0; i < integerArray3.length; i++) {
              console.log(selectedElements[integerArray3[i]].innerHTML);
            }
          }

          break;
        }
      }
      h.click();
    });
    b.appendChild(c);
  }
  x[i].appendChild(b);
  a.addEventListener("click", function (e) {
    /* When the select box is clicked, close any other select boxes,
    and open/close the current select box: */
    e.stopPropagation();
    closeAllSelect(this);
    this.nextSibling.classList.toggle("select-hide");
    this.classList.toggle("select-arrow-active");
  });
}

function closeAllSelect(elmnt) {
  /* A function that will close all select boxes in the document,
  except the current select box: */
  var x,
    y,
    i,
    xl,
    yl,
    arrNo = [];
  x = document.getElementsByClassName("select-items");
  y = document.getElementsByClassName("select-selected");
  xl = x.length;
  yl = y.length;
  for (i = 0; i < yl; i++) {
    if (elmnt == y[i]) {
      arrNo.push(i);
    } else {
      y[i].classList.remove("select-arrow-active");
    }
  }
  for (i = 0; i < xl; i++) {
    if (arrNo.indexOf(i)) {
      x[i].classList.add("select-hide");
    }
  }
}

/* If the user clicks anywhere outside the select box,
then close all select boxes: */
document.addEventListener("click", closeAllSelect);