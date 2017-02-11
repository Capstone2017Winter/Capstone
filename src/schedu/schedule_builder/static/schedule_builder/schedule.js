$(document).ready(function(){
	classes = new Array();
	$('#addClassButton').click(function(){
		var classNameInput = $("#classSearchTextId").val();
		if (classes.indexOf(classNameInput) != -1) {
			alert("This class is already in your list");
			return;
		}
		classes.push(classNameInput);
		$('#classListId').append('<li><p>' + classNameInput + "</p></li>");
	});
});
